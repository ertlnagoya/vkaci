from reg import reg
from dataclasses import dataclass
from typing import Callable, Optional

EXTENTION_APIS = [
    "vkQueuePresentKHR",
]

@dataclass
class Func:
    @dataclass
    class Param:
        ptype: str | None
        name: str
        content: str

    name: str
    ret_type: str
    params: list[Param]
    API_group: str

    @staticmethod
    def from_command(cmd: reg.Command) -> "Func":
        return Func(
            cmd.proto.name,
            cmd.proto.type_,
            [
                Func.Param(
                    p.type_,
                    p.name,
                    p.content,
                )
                for p in cmd.params
            ],
            cmd.APIgroup,
        )

    def define(
        self,
        beforeRun: str | None = None,
        afterRun: str | None = None,
    ) -> str:
        res = []
        res.append('extern "C" PUBLIC')
        res.append(f"VKAPI_ATTR {self.ret_type} VKAPI_CALL VKACI_{self.name[2:]}({', '.join([p.content for p in self.params])})" + " {")
        res.append("  std::lock_guard<std::mutex> lg(global_lock);")
        if beforeRun:
            res.append("  " + beforeRun)
        if self.ret_type != "void":
            if self.API_group == "Instance":
                res.append(f"  {self.ret_type} _result = instance_dispatch[GetKey({self.params[0].name})].{self.name[2:]}({', '.join([p.name for p in self.params])});")
            else:
                res.append(f"  {self.ret_type} _result = device_dispatch[GetKey({self.params[0].name})].{self.name[2:]}({', '.join([p.name for p in self.params])});")
        else:
            if self.API_group == "Instance":
                res.append(f"  instance_dispatch[GetKey({self.params[0].name})].{self.name[2:]}({', '.join([p.name for p in self.params])});")
            else:
                res.append(f"  device_dispatch[GetKey({self.params[0].name})].{self.name[2:]}({', '.join([p.name for p in self.params])});")
        if afterRun:
            res.append("  " + afterRun)
        if self.ret_type != "void":
            res.append("  return _result;")
        res.append("}")
        return "\n".join(res)


@dataclass
class Hook:
    is_target: bool = False
    before_run: str = ""
    after_run: str = ""


HookFunc = Callable[[Func], Hook]


@dataclass
class Hooks:
    header: str
    hook_funcs: list[HookFunc]

    def apply(self, func: Func) -> Optional[str]:
        """
        Apply hooks to the function if it is a target.
        If the function is not a target, return None.
        Before is FIFO, after is LIFO.
        Ex. before[0] -> before[1] -> before[2] -> original -> after[2] -> after[1] -> after[0].
        """

        befores, afters = [], []
        is_target = False

        for hook_func in self.hook_funcs:
            hook = hook_func(func)
            if not hook.is_target:
                continue
            is_target = True
            befores.append(hook.before_run)
            afters.append(hook.after_run)

        if not is_target:
            return None

        return func.define("".join(befores), "".join(reversed(afters)))


def header() -> str:
    return """#include <vulkan/vulkan.h>
#include <vulkan/vk_layer.h>
#include <vulkan/utility/vk_dispatch_table.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <map>
#include <mutex>
"""

def global_things() -> str:
    return """std::mutex global_lock;

template<typename DispatchableType>
void *GetKey(DispatchableType inst)
{
  return *(void **)inst;
}

std::map<void *, VkuInstanceDispatchTable> instance_dispatch;
std::map<void *, VkuDeviceDispatchTable> device_dispatch;

#define PUBLIC __attribute__((visibility("default")))
#define GETPROCADDR(func) if(!strcmp(pName, "vk" #func)) return (PFN_vkVoidFunction)&VKACI_##func;
"""

def filter_funcs(
    registry: reg.Registry,
    filter: Callable[[str, float], bool] = lambda api, version: True,
) -> dict[str, Func]:
    required_cmds: set[str] = set()
    removed_cmds: set[str] = set()
    for feature in registry.features:
        if filter(feature.api, float(feature.number)):
            for req in feature.require:
                required_cmds.update(
                    [elm.name for elm in req.elements if elm.type_ == "command"]
                )
            for rem in feature.remove:
                removed_cmds.update(
                    [elm.name for elm in rem.elements if elm.type_ == "command"]
                )
    target_cmds = required_cmds - removed_cmds

    funcs: dict[str, Func] = {}
    for cmds in registry.commands:
        for cmd in cmds.commands:
            if cmd.proto.name in target_cmds:
                funcs[cmd.proto.name] = Func.from_command(cmd)

    return funcs


# バージョンなどを気にせず全ての関数を返す
def no_filter_funcs(
    registry: reg.Registry,
) -> dict[str, Func]:
    required_cmds: set[str] = set()
    removed_cmds: set[str] = set()
    for feature in registry.features:
        if feature.api != "vulkansc":   #VulkanSCにしか含まれないものは一旦除外
            for req in feature.require:
                required_cmds.update(
                    [elm.name for elm in req.elements if elm.type_ == "command"]
                )
            for rem in feature.remove:
                removed_cmds.update(
                    [elm.name for elm in rem.elements if elm.type_ == "command"]
                )

    for extention in registry.extensions:
        for req in extention.require:
            required_cmds.update(
                [elm.name for elm in req.elements if elm.name in EXTENTION_APIS]
            )
    target_cmds = required_cmds - removed_cmds

    funcs: dict[str, Func] = {}
    for cmds in registry.commands:
        for cmd in cmds.commands:
            if cmd.proto.name in target_cmds:
                funcs[cmd.proto.name] = Func.from_command(cmd)

    return funcs

def define_required_func(funcs: list[Func]):
    res: list[str] = []
    res.extend(
        [
            '\nextern "C" PUBLIC',
            "VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance)",
            "{",
            "  std::lock_guard<std::mutex> lg(global_lock);",
            "  VkLayerInstanceCreateInfo *layerCreateInfo = (VkLayerInstanceCreateInfo *)pCreateInfo->pNext;",
            "  while(layerCreateInfo && (layerCreateInfo->sType != VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO || layerCreateInfo->function != VK_LAYER_LINK_INFO))",
            "  {",
            "    layerCreateInfo = (VkLayerInstanceCreateInfo *)layerCreateInfo->pNext;",
            "  }",
            "  if(layerCreateInfo == NULL)",
            "  {",
            "    return VK_ERROR_INITIALIZATION_FAILED;",
            "  }",
            "  PFN_vkGetInstanceProcAddr gpa = layerCreateInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;",
            "  layerCreateInfo->u.pLayerInfo = layerCreateInfo->u.pLayerInfo->pNext;",
            "  PFN_vkCreateInstance createFunc = (PFN_vkCreateInstance)gpa(VK_NULL_HANDLE, \"vkCreateInstance\");",

            "\n  VkResult ret = createFunc(pCreateInfo, pAllocator, pInstance);\n",

            "  VkuInstanceDispatchTable dispatchTable;",
            "  dispatchTable.GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)gpa(*pInstance, \"vkGetInstanceProcAddr\");",
            "  dispatchTable.DestroyInstance = (PFN_vkDestroyInstance)gpa(*pInstance, \"vkDestroyInstance\");",
        ]
    )

    for fn in funcs:
        if fn.API_group == "Instance":
            res.extend([
                f"  dispatchTable.{fn.name[2:]} = (PFN_{fn.name})gpa(*pInstance, \"{fn.name}\");",
            ])

    res.extend(
        [
            "  instance_dispatch[GetKey(*pInstance)] = dispatchTable;",
            "  return VK_SUCCESS;",
            "}",
        ]
    )

    res.extend(
        [
            '\nextern "C" PUBLIC',
            "VKAPI_ATTR void VKAPI_CALL VKACI_DestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator)",
            "{",
            "  std::lock_guard<std::mutex> lg(global_lock);",
            "  instance_dispatch.erase(GetKey(instance));",
            "}",
        ]
    )

    res.extend(
        [
            '\nextern "C" PUBLIC',
            "VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice)",
            "{",
            "  std::lock_guard<std::mutex> lg(global_lock);",
            "  VkLayerDeviceCreateInfo *layerCreateInfo = (VkLayerDeviceCreateInfo *)pCreateInfo->pNext;",
            "  while(layerCreateInfo && (layerCreateInfo->sType != VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO || layerCreateInfo->function != VK_LAYER_LINK_INFO))",
            "  {",
            "    layerCreateInfo = (VkLayerDeviceCreateInfo *)layerCreateInfo->pNext;",
            "  }",
            "  if(layerCreateInfo == NULL)",
            "  {",
            "    return VK_ERROR_INITIALIZATION_FAILED;",
            "  }",
            "  PFN_vkGetInstanceProcAddr gipa = layerCreateInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;",
            "  PFN_vkGetDeviceProcAddr gdpa = layerCreateInfo->u.pLayerInfo->pfnNextGetDeviceProcAddr;",
            "  layerCreateInfo->u.pLayerInfo = layerCreateInfo->u.pLayerInfo->pNext;",
            "  PFN_vkCreateDevice createFunc = (PFN_vkCreateDevice)gipa(VK_NULL_HANDLE, \"vkCreateDevice\");",

            "\n  VkResult ret = createFunc(physicalDevice, pCreateInfo, pAllocator, pDevice);\n",

            "  VkuDeviceDispatchTable dispatchTable;",
            "  dispatchTable.GetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)gdpa(*pDevice, \"vkGetDeviceProcAddr\");",
            "  dispatchTable.DestroyDevice = (PFN_vkDestroyDevice)gdpa(*pDevice, \"vkDestroyDevice\");",
        ]
    )

    for fn in funcs:
        if fn.API_group == "Device":
            res.extend([
                f"  dispatchTable.{fn.name[2:]} = (PFN_{fn.name})gdpa(*pDevice, \"{fn.name}\");",
            ])

    res.extend(
        [
            "  device_dispatch[GetKey(*pDevice)] = dispatchTable;",
            "  return VK_SUCCESS;",
            "}",
        ]
    )

    res.extend(
        [
            '\nextern "C" PUBLIC',
            "VKAPI_ATTR void VKAPI_CALL VKACI_DestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator)",
            "{",
            "  std::lock_guard<std::mutex> lg(global_lock);",
            "  device_dispatch.erase(GetKey(device));",
            "}",
        ]
    )

    return "\n".join(res)

def define_get_proc_addr_func_device(get_proc_func: Func, funcs: list[Func]):
    res: list[str] = []
    res.extend(
        [
            'extern "C" PUBLIC',
            f"VKAPI_ATTR {get_proc_func.ret_type} VKAPI_CALL VKACI_{get_proc_func.name[2:]}({', '.join([p.content for p in get_proc_func.params])})",
            "{",
            "  GETPROCADDR(GetDeviceProcAddr);",
            "  GETPROCADDR(DestroyDevice);"
        ]
    )

    for fn in funcs:
        if not fn.API_group == "Instance":
            res.extend(
                [
                    f"  GETPROCADDR({fn.name[2:]});",
                ]
            )

    res.extend(
        [
            f"  return device_dispatch[GetKey({get_proc_func.params[0].name})].GetDeviceProcAddr({get_proc_func.params[0].name}, {get_proc_func.params[1].name});",
            "}",
        ]
    )
    return "\n".join(res)

def define_get_proc_addr_func_instance(get_proc_func: Func, funcs: list[Func]):
    res: list[str] = []
    res.extend(
        [
            'extern "C" PUBLIC',
            f"VKAPI_ATTR {get_proc_func.ret_type} VKAPI_CALL VKACI_{get_proc_func.name[2:]}({', '.join([p.content for p in get_proc_func.params])})",
            "{",
            "  GETPROCADDR(GetInstanceProcAddr);",
            "  GETPROCADDR(GetDeviceProcAddr);",
            "  GETPROCADDR(CreateInstance);",
            "  GETPROCADDR(DestroyInstance);",
            "  GETPROCADDR(CreateDevice);",
        ]
    )

    for fn in funcs:
        res.extend(
            [
                f"  GETPROCADDR({fn.name[2:]});",
            ]
        )

    res.extend(
        [
            f"  return instance_dispatch[GetKey({get_proc_func.params[0].name})].GetInstanceProcAddr({get_proc_func.params[0].name}, {get_proc_func.params[1].name});",
            "}",
        ]
    )
    return "\n".join(res)