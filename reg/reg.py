from dataclasses import dataclass
from typing import Literal, cast
import xml.etree.ElementTree as ET


def attr_not_found_error(elm: str, attr: str) -> ValueError:
    return ValueError(f"{elm} element must have a {attr} attribute")


def elm_not_found_error(parentelm: str, elm: str) -> ValueError:
    return ValueError(f"{elm} element not found in {parentelm}")


@dataclass
class Type:
    name: str
    content: str
    requires: str | None
    api: str | None

    @staticmethod
    def from_xml(element: ET.Element) -> "Type":
        name_atr = element.get("name")
        name_eml = element.find("name")

        if name_atr is not None and name_eml is not None:
            raise ValueError(
                "Type element must have either a name attribute or name element"
            )
        elif name_atr is not None:
            name = name_atr
        elif name_eml is not None:
            if name_eml.text is None:
                raise ValueError("Name element in Type must have text")
            name = name_eml.text
        else:
            raise ValueError("Type element must have a name attribute or name element")

        return Type(
            name=name,
            content="".join(element.itertext()),
            requires=element.get("requires"),
            api=element.get("api"),
        )


@dataclass
class Group:
    name: str
    enums: list[str]

    @staticmethod
    def from_xml(element: ET.Element) -> "Group":
        name = element.get("name")
        if name is None:
            raise attr_not_found_error("Group", "name")

        enums = []
        for enum in element.findall("enum"):
            enum_name = enum.get("name")
            if enum_name is None:
                raise attr_not_found_error("Goup/Enum", "name")
            enums.append(enum_name)

        return Group(name=name, enums=enums)


@dataclass
class Enum:
    name: str
    value: str | None   #valueがない場合がある
    api: str | None
    type_: str | None
    group: str | None
    alias: str | None

    @staticmethod
    def from_xml(element: ET.Element) -> "Enum":
        name = element.get("name")
        if name is None:
            raise attr_not_found_error("Enum", "name")

        value = element.get("value")
        # if value is None:
        #     raise attr_not_found_error("Enum", "value")

        return Enum(
            name=name,
            value=value,
            api=element.get("api"),
            type_=element.get("type"),
            group=element.get("group"),
            alias=element.get("alias"),
        )


@dataclass
class Unused:
    start: str
    end: str | None
    vendor: str | None

    @staticmethod
    def from_xml(element: ET.Element) -> "Unused":
        start = element.get("start")
        if start is None:
            raise attr_not_found_error("Unused", "start")

        return Unused(start=start, end=element.get("end"), vendor=element.get("vendor"))


@dataclass
class Enums:
    namespace: str | None
    group: str | None
    type_: str | None
    start: str | None
    end: str | None
    vendor: str | None
    enums: list[Enum | Unused]

    @staticmethod
    def from_xml(element: ET.Element) -> "Enums":
        enums: list[Enum | Unused] = []
        for enum in element:
            if enum.tag == "enum":
                enums.append(Enum.from_xml(enum))
            elif enum.tag == "unused":
                enums.append(Unused.from_xml(enum))
            elif enum.tag == "comment":
                continue
            else:
                raise ValueError(f"Unknown element {enum.tag} in Enums")

        return Enums(
            namespace=element.get("namespace"),
            group=element.get("group"),
            type_=element.get("type"),
            start=element.get("start"),
            end=element.get("end"),
            vendor=element.get("vendor"),
            enums=enums,
        )


@dataclass
class Proto:
    type_: str
    name: str
    group: str | None
    kind: str | None
    class_: str | None

    @staticmethod
    def from_xml(element: ET.Element) -> "Proto":
        type_element = element.find("type")
        if type_element is None:
            raise attr_not_found_error("Proto", "type")
        type_ = type_element.text
        if type_ is None:
            raise ValueError("Type element in Proto must have text")

        name_element = element.find("name")
        if name_element is None:
            raise attr_not_found_error("Proto", "name")
        name = name_element.text
        if name is None:
            raise ValueError("Name element in Proto must have text")

        return Proto(
            type_=type_,
            name=name,
            group=element.get("group"),
            kind=element.get("kind"),
            class_=element.get("class"),
        )
    
    @staticmethod
    def dummy() -> "Proto":     #aliasしか持たないcommandに返すためのダミー
        return Proto(
            type_= "",
            name="",
            group="",
            kind="",
            class_="",
        )


@dataclass
class Param:
    content: str
    type_: str
    name: str
    group: str | None
    kind: str | None
    class_: str | None
    len: str | None

    @staticmethod
    def from_xml(element: ET.Element) -> "Param":
        type_element = element.find("type")
        if type_element is None:
            raise attr_not_found_error("Param", "type")
        type_ = type_element.text
        if type_ is None:
            raise ValueError("Type element in Param must have text")

        name_element = element.find("name")
        if name_element is None:
            raise attr_not_found_error("Param", "name")
        name = name_element.text
        if name is None:
            raise ValueError("Name element in Param must have text")

        return Param(
            content="".join(element.itertext()),
            type_=type_,
            name=name,
            group=element.get("group"),
            kind=element.get("kind"),
            class_=element.get("class"),
            len=element.get("len"),
        )


@dataclass
class Command:
    proto: Proto
    params: list[Param]
    alias: str | None
    name: str
    vecequiv: str | None
    APIgroup: str

    @staticmethod
    def from_xml(element: ET.Element) -> "Command":
        proto_element = element.find("proto")
        proto = Proto.dummy()
        if proto_element is not None:
            proto = Proto.from_xml(proto_element)

        param_elements = element.findall("param")
        if param_elements is None:
            raise elm_not_found_error("Command", "param")
        params = [Param.from_xml(param) for param in param_elements]

        name = element.get("name")
        if name is None:
            name = ""

        # そのAPIがデバイスレベルかインスタンスレベルかの判定
        APIgroup = ""
        if proto.name == "vkGetInstanceProcAddr" or proto.name == "vkGetDeviceProcAddr":
            APIgroup = "Get_Proc_Funcs"
        elif proto.name.startswith("vkEnumerate") or proto.name == "vkCreateInstance" or proto.name == "vkCreateDevice":
            APIgroup = "Instance"
        elif proto.name == "vkGetExternalComputeQueueDataNV":
            APIgroup = "Device"
        elif len(params) > 0:
            if params[0].type_ == "VkDevice" or params[0].type_ == "VkQueue" or params[0].type_ == "VkCommandBuffer":
                APIgroup = "Device"
            elif params[0].type_ == "VkInstance" or params[0].type_ == "VkPhysicalDevice":
                APIgroup = "Instance"
        
        if APIgroup == "" and proto.name:
            raise ValueError(f"APIgroup element in Command must be judged:{proto.name}")

        return Command(
            proto=proto,
            params=params,
            alias=element.get("alias"),
            name=name,
            vecequiv=element.get("vecequiv"),
            APIgroup=APIgroup,
        )


@dataclass
class Commands:
    namespace: str | None
    commands: list[Command]

    @staticmethod
    def from_xml(element: ET.Element) -> "Commands":

        #aliasの変換

        commands = [Command.from_xml(cmd) for cmd in element.findall("command")]
        for command in commands:
            if command.alias != "":
                for c in commands:
                    if c.proto.name == command.alias:
                        command.proto = c.proto
                        command.proto.name = command.name
                        command.params = c.params
                        command.vecequiv = c.vecequiv
                        command.APIgroup = c.APIgroup
                        break
        return Commands(
            namespace=element.get("namespace"),
            commands=commands,
        )


@dataclass
class InterfaceElement:
    type_: Literal["type", "command", "enum", "feature", "comment"] #featureの場合がある&commentの場合nameを持たない
    name: str

    @staticmethod
    def from_xml(element: ET.Element) -> "InterfaceElement":
        type_ = element.tag
        if type_ not in ["type", "command", "enum", "feature", "comment"]:
            raise ValueError(f"Unknown element type {type_} in InterfaceElement")

        name = element.get("name")
        if name is None and type_ != "comment":
            raise attr_not_found_error(f"InterfaceElement/{type_}", "name")
        elif name == None:
            name = ""

        return InterfaceElement(
            type_=cast(Literal["type", "command", "enum", "feature", "comment"], type_),
            name=name,
        )


@dataclass
class Interfaces:
    api: str | None
    profile: str | None
    elements: list[InterfaceElement]

    @staticmethod
    def from_xml(element: ET.Element) -> "Interfaces":
        return Interfaces(
            api=element.get("api"),
            profile=element.get("profile"),
            elements=[InterfaceElement.from_xml(elm) for elm in element if InterfaceElement.from_xml(elm).name != ""],
        )


@dataclass
class Feature:
    api: str
    name: str
    number: str
    protect: str | None
    require: list[Interfaces]
    remove: list[Interfaces]
    #deprecateというタグもあるっぽい？

    @staticmethod
    def from_xml(element: ET.Element) -> "Feature":
        api = element.get("api")
        if api is None:
            raise attr_not_found_error("Feature", "api")

        name = element.get("name")
        if name is None:
            raise attr_not_found_error("Feature", "name")

        number = element.get("number")
        if number is None:
            raise attr_not_found_error("Feature", "number")

        return Feature(
            api=api,
            name=name,
            number=number,
            protect=element.get("protect"),
            require=[Interfaces.from_xml(req) for req in element.findall("require")],
            remove=[Interfaces.from_xml(rem) for rem in element.findall("remove")],
        )


@dataclass
class Extension:
    name: str
    supported: str | None
    protect: str | None
    # promotedto: str | None
    require: list[Interfaces]
    remove: list[Interfaces]

    @staticmethod
    def from_xml(element: ET.Element) -> "Extension":
        name = element.get("name")
        if name is None:
            raise attr_not_found_error("Extension", "name")

        return Extension(
            name=name,
            supported=element.get("supported"),
            protect=element.get("protect"),
            # promotedto=element.get("promotedto"),
            require=[Interfaces.from_xml(req) for req in element.findall("require")],
            remove=[Interfaces.from_xml(rem) for rem in element.findall("remove")],
        )


@dataclass
class Registry:
    types: list[Type]
    groups: list[Group]
    enums: list[Enums]
    commands: list[Commands]
    features: list[Feature]
    extensions: list[Extension]

    @staticmethod
    def from_xml(element: ET.Element) -> "Registry":
        return Registry(
            types=[Type.from_xml(type_el) for type_el in element.findall("types/type")],
            groups=[
                Group.from_xml(group_el) for group_el in element.findall("groups/group")
            ],
            enums=[Enums.from_xml(enum_el) for enum_el in element.findall("enums")],
            commands=[
                Commands.from_xml(cmd_el) for cmd_el in element.findall("commands")
            ],
            features=[
                Feature.from_xml(feature_el)
                for feature_el in element.findall("feature")
            ],
            extensions=[
                Extension.from_xml(ext_el)
                for ext_el in element.findall("extensions/extension")
            ],
        )

    @staticmethod
    def from_file(source: str) -> "Registry":
        tree = ET.parse(source)
        root = tree.getroot()
        if root.tag != "registry":
            raise ValueError("Root element must be <registry>")
        return Registry.from_xml(root)
