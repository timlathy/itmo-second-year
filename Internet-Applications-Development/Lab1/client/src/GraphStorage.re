type graphLine = {type_: string, x1: string, x2: string, x3: string,
                                 y1: string, y2: string, y3: string};

type graph = {name: string, lines: array(graphLine)};

/* JSON */

[@bs.scope "JSON"] [@bs.val]
external parseGraphArray : string => array(graph) = "parse";

[@bs.scope "JSON"] [@bs.val]
external jsonStringify : 'a => string = "stringify";

/* Local storage */

let localStorageSet = (key: string, value: string): unit =>
  Dom.Storage.localStorage |> Dom.Storage.setItem(key, value);

let localStorageGet = (key: string): option(string) =>
  Dom.Storage.localStorage |> Dom.Storage.getItem(key);

let storageKey = "saved_graphs";

let save = (graph: graph): unit => {
  storageKey
  |> localStorageGet
  |> (fun
    | Some(json) => parseGraphArray(json)
    | _ => [||])
  |> Js.Array.concat([|graph|])
  |> jsonStringify
  |> localStorageSet(storageKey);
};

