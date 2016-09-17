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

module Dyld = struct
  external image_count : unit -> int = "caml_dyld_image_count"
  external image_name : int -> string option = "caml_dyld_image_name"
  let images () =
    let all = ref [] in
    for i = 0 to image_count () - 1 do
      match image_name i with None -> () | Some s -> all := s :: !all
    done;
    List.rev !all
  (* FixME, the integer sizing is wrong *)
  (* external image_slide : int -> int = "caml_dyld_image_slide" *)
  (* external version_of_runtime_library : string -> int option = "caml_dyld_image_version" *)
end
