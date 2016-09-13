let () =
  ObjC.Introspect.properties "NSString"
  |> List.iter print_endline;

  match ObjC.Introspect.protocols "NonExistent" with
  | [] -> print_endline "Doesn't exist so empty list"
  | l -> l |> List.iter print_endline
