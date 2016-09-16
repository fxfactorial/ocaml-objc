Objective-C from OCaml
=========================

Manipulate and introspect `Objective-C` from `OCaml`!

Right now this is using a high level interface provided by
[DLIntrospection](https://github.com/garnett/DLIntrospection).

I made this because its a lot easier to play with `Objective-C`,
investigate it from a repl, use `utop`, than the compile cycle,
motivated in part by my `iOS` reverse engineering work.

# Usage

Look at `example/e.ml` for an example of the high level `Introspect`
module. Empty lists mean that the `Objective-C` side didn't know about
the class in question.

```ocaml
let () =
  ObjC.Introspect.properties "NSString"
  |> List.iter print_endline;

  match ObjC.Introspect.protocols "NonExistent" with
  | [] -> print_endline "Doesn't exist so empty list"
  | l -> l |> List.iter print_endline
```

with `utop`, you can do:

```ocaml
#require "objc";;
let props = ObjC.Introspect.properties "NSString";;
```

Or if you feel like compiling (use `ocamlc` or `ocamlopt`):

```shell
$ ocamlfind ocamlc -package objc e.ml -linkpkg -o T
```
