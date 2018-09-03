<?php include __DIR__ . '/../_head.html.php'; ?>

<section class="graph-header">
  <div id="js-pip-preview" class="graph-header__preview"></div>
  <h1 id="js-pip-graph-name" class="graph-header__title"><?=$title?></h1>
</section>

<form id="js-pip-form" method="get" action="/graphs/point_in_polygon">
  <div class="column-layout">
    <div class="column-layout__fixed">
      <h2>Point coordinates</h2>
      <fieldset class="card">
        <label class="card__title">X</label>
        <?php foreach (["-2", "-1.5", "-1", "-0.5", "0", "0.5", "1", "1.5", "2"] as $i => $v) { ?>
        <label><input type="checkbox" name="X[<?=$i?>]" value="<?=$v?>" /><?=$v?></label>
        <?php } ?>

        <label class="card__title card__title--extra" for="y">Y</label>
        <input type="number" class="input" data-input="Y" min="-5" max="3" step="0.5" placeholder="0"
               id="Y" name="Y" />
      </fieldset>
      <h2>Graph variables</h2>
      <fieldset class="card">
        <?php foreach ($vars as $var) { ?>
          <label class="card__title" for="v_<?=$var?>"><?=$var?></label>
          <input type="number" class="input" data-input="<?=$var?>" min="1" max="4"
                 id="v_<?=$var?>" name="v[<?=$var?>]" value="1" step="0.5" />
        <?php } ?>
      </fieldset>
      <button class="button button--primary" type="submit">Compute</button>
    </div>
    <div class="column-layout__gap"></div>
    <div class="column-layout__flexible">
      <h2>History<button id="js-pip-history-clear" class="button button--inline button--trash"></button></h2>
      <section class="card card--error hidden" id="js-error-container"></section>
      <section class="pip-history" id="js-pip-history"></section>
      <section class="card card--info pip-history-tip">
        <p>The X and Y axes range from -5 to 5, with a floating point precision of 0.1.</p>
        <p><em>R</em> is restricted to a [1; 4] range; <em>Y</em> is restricted to a [-5; 3] range.</p>
      </section>
    </div>
  <input type="hidden" name="g" value="<?=
    htmlspecialchars($graph, ENT_QUOTES) ?>" />
</form>

<?php include __DIR__ . '/../_footer.html.php'; ?>
