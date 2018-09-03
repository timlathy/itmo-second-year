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

[@bs.scope "JSON"] [@bs.val]
external historyFromJson : string => array(string) = "parse";

[@bs.scope "JSON"] [@bs.val]
external historyAsJson : array(string) => string = "stringify";

/* Local storage */

let localStorageSet = (key: string, value: string): unit =>
  Dom.Storage.localStorage |> Dom.Storage.setItem(key, value);

let localStorageGet = (key: string): option(string) =>
  Dom.Storage.localStorage |> Dom.Storage.getItem(key);

let localStorageRemove = (key: string): unit =>
  Dom.Storage.localStorage |> Dom.Storage.removeItem(key);

let storageKey = "saved_graphs";

let load = (): array(graph) =>
  storageKey
  |> localStorageGet
  |> fun
    | Some(json) => graphsFromJson(json)
    | _ => [||];

let append = (graph: graph): unit =>
  load()
  |> Js.Array.concat([|graph|])
  |> graphsAsJson
  |> localStorageSet(storageKey);

let nameExists = (name: string): bool =>
  load() |> Js.Array.some((g) => g.name == name);

let previewKey = (graphName: string) =>
  "graph_preview_" ++ Js.Global.encodeURIComponent(graphName);

let loadPreviewByName = (graphName: string): string =>
  switch (graphName |> previewKey |> localStorageGet) {
  | Some(preview) => preview
  | _ => ""
  };

let setPreviewByName = (graphName: string, preview: string): unit =>
  localStorageSet(previewKey(graphName), preview);

let savedInputKey = (graphNameEnc: string, inputName: string): string =>
  {j|saved_input_$(graphNameEnc)_$(inputName)|j};

let setInputValue = (graphNameEnc: string, inputName: string, value: string): unit =>
  localStorageSet(savedInputKey(graphNameEnc, inputName), value);

let getInputValue = (graphNameEnc: string, inputName: string): option(string) =>
  localStorageGet(savedInputKey(graphNameEnc, inputName));

let toggleInput = (graphNameEnc: string, inputName: string, checked: bool): unit => {
  let key = savedInputKey(graphNameEnc, inputName);

  if (checked) localStorageSet(key, "checked") else localStorageRemove(key);
};

let isInputChecked = (graphNameEnc: string, inputName: string): bool =>
  switch (localStorageGet(savedInputKey(graphNameEnc, inputName))) {
  | Some(_) => true
  | _ => false
  };

let loadHistory = (graphNameEnc: string): array(string) =>
  {j|hist_$(graphNameEnc)|j}
  |> localStorageGet
  |> fun
    | Some(json) => historyFromJson(json)
    | _ => [||];

let appendHistory = (graphNameEnc: string, item: string): unit =>
  graphNameEnc
  |> loadHistory
  |> Js.Array.concat([|item|])
  |> historyAsJson
  |> localStorageSet({j|hist_$(graphNameEnc)|j});

let clearHistory = (graphNameEnc: string): unit =>
  localStorageRemove({j|hist_$(graphNameEnc)|j});
