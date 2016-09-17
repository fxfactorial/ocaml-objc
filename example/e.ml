let () =
  ObjC.Introspect.properties "NSString"
  |> List.iter print_endline;

  match ObjC.Introspect.protocols "NonExistent" with
  | [] -> print_endline "Doesn't exist so empty list"
  | l -> l |> List.iter print_endline

let () = ObjC.(
    Dyld.image_count () |> string_of_int |> print_endline;
    for i = 0 to Dyld.image_count () - 1 do
      match Dyld.image_name i with
      | None -> ()
      | Some s -> Printf.sprintf "Image:%s" s |> print_endline;
      (* Printf.sprintf "Image:%s at slide:%d" (Dyld.image_name i) (Dyld.image_slide i) *)
      (* |> print_endline *)
    done;
    (* match Dyld.version_of_runtime_library "ChineseTokenizer" with *)
    (* | None -> print_endline "Unknown library" *)
    (* | Some i -> print_endline (string_of_int i) *)
  )

