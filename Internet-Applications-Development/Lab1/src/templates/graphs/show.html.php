<?php include __DIR__ . '/../_head.html.php'; ?>

<section class="graph-header">
  <div id="js-pip-preview" class="graph-header__preview"></div>
  <h1 id="js-pip-graph-name" class="graph-header__title"><?=$title?></h1>
</section>

<form id="js-pip-form" method="get" action="/graphs/point_in_polygon">
  <div class="column-layout">
    <div class="column-layout__fixed column-layout__fixed--narrow">
      <h2>Point coordinates</h2>
      <fieldset class="card">
        <div class="card__title">X</div>
        <?php foreach (["-2", "-1.5", "-1", "-0.5", "0", "0.5", "1", "1.5", "2"] as $i => $v) { ?>
        <label><input type="checkbox" name="X[<?=$i?>]" value="<?=$v?>" /><?=$v?></label>
        <?php } ?>

        <div class="card__title card__title--extra">Y</div>
        <input type="number" name="Y" value="0" step="0.5" /> 
      </fieldset>
      <h2>Graph variables</h2>
      <fieldset class="card">
        <?php foreach ($vars as $var) { ?>
          <label class="input-label" for="v_<?=$var?>"><?=$var?></label>
          <input class="input" id="v_<?=$var?>" name="v[<?=$var?>]" placeholder="0" />
        <?php } ?>
      </fieldset>
      <button class="button button--primary" type="submit">Compute</button>
    </div>
    <div class="column-layout__gap"></div>
    <div class="column-layout__flexible">
      <h2>History<button id="js-pip-history-clear" class="button button--inline button--trash"></button></h2>
      <section class="card card--error hidden" id="js-error-container"></section>
      <section id="js-pip-history"></section>
    </div>
  <input type="hidden" name="g" value="<?=
    htmlspecialchars($graph, ENT_QUOTES) ?>" />
</form>

<?php include __DIR__ . '/../_footer.html.php'; ?>
