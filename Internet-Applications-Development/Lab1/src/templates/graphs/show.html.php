<?php include __DIR__ . '/../_head.html.php'; ?>

<?php foreach ($variables as $var) { ?>
  <input name="<?=$var?>" placeholder="<?=$var?>" />
<?php } ?>

<div id="js-graph-view"></div>

<?php include __DIR__ . '/../_footer.html.php'; ?>
