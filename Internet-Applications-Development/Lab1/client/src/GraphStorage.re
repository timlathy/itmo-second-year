/* NOTE: Server-side code depends on the declaration order in record types. */

type graphLine = {type_: string, x1: string, x2: string, x3: string,
                                 y1: string, y2: string, y3: string};

type graph = {name: string, lines: array(graphLine)};

/* JSON */

[@bs.scope "JSON"] [@bs.val]
external graphsFromJson : string => array(graph) = "parse";

[@bs.scope "JSON"] [@bs.val]
external graphAsJson : graph => string = "stringify";

[@bs.scope "JSON"] [@bs.val]
external graphsAsJson : array(graph) => string = "stringify";

/* Local storage */

let localStorageSet = (key: string, value: string): unit =>
  Dom.Storage.localStorage |> Dom.Storage.setItem(key, value);

let localStorageGet = (key: string): option(string) =>
  Dom.Storage.localStorage |> Dom.Storage.getItem(key);

let storageKey = "saved_graphs";

let load = (): array(graph) => {
  storageKey
  |> localStorageGet
  |> fun
    | Some(json) => graphsFromJson(json)
    | _ => [||]
};

let append = (graph: graph): unit => {
  load()
  |> Js.Array.concat([|graph|])
  |> graphsAsJson
  |> localStorageSet(storageKey);
};
