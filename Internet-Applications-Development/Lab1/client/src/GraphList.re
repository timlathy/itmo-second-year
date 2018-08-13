let graphItemHtml = (g: GraphStorage.graph): string => {
  let numLines = Js.Array.length(g.lines);
  let graphURI = g |> GraphStorage.graphAsJson |> Js.Global.encodeURIComponent;

  {j|<div class="graph-list__item">|j} ++
    "<div>" ++ g.name ++ {j|, $numLines lines|j} ++ "</div>" ++
    {j|<a href="/graphs/show?g=$graphURI">Open</a>|j} ++
  "</div>"
};

let init = () => Page.setupElementById("js-graph-list", (graphList) => {
  GraphStorage.load()
  |> Js.Array.map(graphItemHtml)
  |> Js.Array.joinWith("")
  |> Webapi.Dom.Element.setInnerHTML(graphList);
});
