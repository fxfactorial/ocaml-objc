Objective-C from OCaml
=========================

Manipulate and introspect `Objective-C` from `OCaml`!

Right now this is using a high level interface provided by
[DLIntrospection](https://github.com/garnett/DLIntrospection).

I made this because its a lot easier to play with `Objective-C`,
investigate it from a repl, use `utop`, than the compile cycle,
motivated in part by my `iOS` reverse engineering work.

# Installation

Install with `opam`, the OCaml package manager! Get a crash course on
`OCaml`, `opam`
[here](http://hyegar.com/2015/10/20/so-youre-learning-ocaml/)

```shell
$ opam install objc
```

or you get a latest version directly from the git repo: 

```shell
$ opam pin add objc git@github.com:fxfactorial/ocaml-objc.git
```

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

Also exposes some parts of the `dyld` API, the OSX, iOS linker.

with `utop`, you can do:

```ocaml
#require "objc";;
let imgs = ObjC.Dyld.images ();;
al imgs : string list =
["/Users/Edgar/.opam/objc/bin/ocamlrun"; "/usr/lib/libncurses.5.4.dylib";
 "/usr/lib/libSystem.B.dylib"; "/usr/lib/system/libcache.dylib";
 "/usr/lib/system/libcommonCrypto.dylib";
 "/usr/lib/system/libcompiler_rt.dylib"; 
 "/usr/lib/system/libcopyfile.dylib";
 ...
```


```ocaml
let props = ObjC.Introspect.properties "NSString";;
val props : string list = 
["@property (atomic, assign, readonly) Q length"; 
"@property (atomic, copy, readonly) NSString* stringByRemovingPercentEncoding"]
```

Or if you feel like compiling (use `ocamlc` or `ocamlopt`):

```shell
$ ocamlfind ocamlc -package objc e.ml -linkpkg -o T
```
