let graphItemHtml = (g: GraphStorage.graph): string => {
  let numLines = Js.Array.length(g.lines);
  let graphName = g.name;
  let preview = GraphStorage.loadPreviewByName(graphName);
  let graphUrl = GraphView.graphUrl(g);

  {j|<div class="card graph-item">|j} ++
    {j|<div class="graph-item__preview">$preview</div>|j} ++
    {j|<div class="graph-item__info">|j} ++
      {j|<div class="card__title">$graphName, $numLines lines</div>|j} ++
      {j|<a href="$graphUrl" class="button">Open</a>|j} ++
    "</div>" ++
  "</div>"
};

let init = () => Page.setupElementById("js-graph-list", (graphList) => {
  GraphStorage.load()
  |> Js.Array.map(graphItemHtml)
  |> Js.Array.joinWith("")
  |> Webapi.Dom.Element.setInnerHTML(graphList);
});
