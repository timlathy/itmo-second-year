<?php include __DIR__ . '/../_head.html.php'; ?>

<form id="line-input-form">
  <input type="text" class="title-input" name="graph-name" placeholder="New Graph" />
  <div class="column-layout">
    <div class="column-layout__fixed column-layout__fixed--narrow">
      <h2>Lines</h2>
      <div id="line-input-container"></div>
    </div>
    <div class="column-layout__gap"></div>
    <div class="column-layout__flexible">
      <h2>Preview<button id="line-input-preview" class="button button--inline button--refresh"></button></h2>
      <div class="graph-preview" id="line-input-preview-container"></div>
      <label class="input-label" for="line-input-variables">Preview variables</label>
      <input class="input" type="text" id="line-input-variables" name="variables" placeholder="R = 2.3, S = ..." />
      <button id="line-input-save" class="button button--primary" type="submit">Save</button>
    </div>
  </div>
</form>

<?php include __DIR__ . '/../_footer.html.php'; ?>
