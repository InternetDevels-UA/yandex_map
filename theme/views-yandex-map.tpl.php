<?php if (!empty($title)): ?>
  <h3><?php print $title; ?></h3>
<?php endif; ?>
<?php if (!empty($rows)): ?>
  <?php foreach ($rows as $id => $row): ?>
    <div class="<?php print $classes_array[$id]; ?>">
      <?php print $row; ?>
    </div>
  <?php endforeach; ?>
<?php endif; ?>

<div id="<?php print $map['map_id']; ?>" style="width: <?php print $map['width']; ?>; height: <?php print $map['height']; ?>"></div>
