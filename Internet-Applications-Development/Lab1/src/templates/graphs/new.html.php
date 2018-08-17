<?php include __DIR__ . '/../_head.html.php'; ?>

<form id="line-input-form">
  <input type="text" class="title-input" name="graph-name" placeholder="Untitled" />
  <div class="column-layout">
    <div class="column-layout__fixed column-layout__fixed--narrow">
      <div id="line-input-container"></div>
    </div>
    <div class="column-layout__gap"></div>
    <div class="column-layout__flexible">
      <fieldset class="fieldset">
        <div class="fieldset__faux-legend">Preview</div>
        <div id="line-input-preview-container"></div>
        <label class="input-label" for="line-input-variables">Preview variables</label>
        <input class="input" type="text" id="line-input-variables" name="variables" placeholder="R = 2.3, S = ..." />
      </fieldset>
      <button id="line-input-preview" class="button">Preview</button>
      <button id="line-input-save" class="button button--primary" type="submit">Save</button>
    </div>
  </div>
</form>

<?php include __DIR__ . '/../_footer.html.php'; ?>
