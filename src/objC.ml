module Introspect = struct

  external classes : unit -> string list = "caml_objc_all_classes"
  external properties : string -> string list = "caml_objc_properties"
  external instance_variables : string -> string list = "caml_objc_instance_variables"
  external class_methods : string -> string list = "caml_objc_class_methods"
  external instance_methods : string -> string list = "caml_objc_instance_methods"
  external protocols : string -> string list = "caml_objc_protocols"
  external parent_class_hierarchy :
    string -> string option = "caml_objc_parent_class_hierarchy"

end
