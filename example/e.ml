let () =
  ObjC.Introspect.properties "NSString"
  |> List.iter print_endline;

  match ObjC.Introspect.protocols "NonExistent" with
  | [] -> print_endline "Doesn't exist so empty list"
  | l -> l |> List.iter print_endline

let () = ObjC.(
    (* Dyld.image_count () |> string_of_int |> print_endline; *)

    (* for i = 0 to Dyld.image_count () - 1 do *)
    (*   match Dyld.image_name i with *)
    (*   | None -> () *)
    (*   | Some s -> Printf.sprintf "Image:%s" s |> print_endline; *)
    (* done; *)

    (* Printf.sprintf "0x%nx is the slide for this binary" (Dyld.image_slide 0) *)
    (* |> print_endline; *)


    let nsstring = nsstring_of_string "Hello World" in
    match call_method nsstring ~selector:"uppercaseString" ~args:[||] with
    | None -> print_endline "Method didn't work"
    | Some e -> print_endline (description e)


    (* match create_object "NSMutableString" with *)
    (* | None -> print_endline "Objective-C couldn't allocate" *)
    (* | Some o -> *)
    (*   call_method o ~selector:"setString:" ~args:[|nsstring_of_string "catdog"|] |> ignore; *)

    (* match Dyld.version_of_runtime_library "ChineseTokenizer" with *)
    (* | None -> print_endline "Unknown library" *)
    (* | Some i -> print_endline (string_of_int i) *)
  )

