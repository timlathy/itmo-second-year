<?php include __DIR__ . '/../_head.html.php'; ?>

<form method="get" action="/graphs/point_in_polygon">
  <?php foreach ($vars as $var) { ?>
    <input name="v[<?=$var?>]" placeholder="<?=$var?>" />
  <?php } ?>

  <div id="js-graph-view"></div>

  <fieldset><legend>X</legend>
    <label><input type="checkbox" name="X[0]" value="-2" /> -2 </label>
    <label><input type="checkbox" name="X[1]" value="-1.5" /> -1.5 </label>
  </fieldset>

  <fieldset><legend>Y</legend>
    <input type="number" name="Y" value="0" step="0.5" /> 
  </fieldset>

  <input type="hidden" name="g" value="<?=
    htmlspecialchars($graph, ENT_QUOTES) ?>" />

  <button type="submit">Compute</button>
</form>

<?php include __DIR__ . '/../_footer.html.php'; ?>
