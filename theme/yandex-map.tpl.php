<?php if (!empty($content)): ?>
  <div class="content"<?php print $content_attributes; ?>>
    <?php
      // We hide the comments and links now so that we can render them later.
      hide($content['comments']);
      hide($content['links']);
      print render($content);
    ?>
  </div>
  
  <div class="links">
    <?php print render($content['links']); ?>
  </div>
<?php endif; ?>

<div id="<?php print $map['map_id']; ?>" style="width: <?php print $map['width']; ?>; height: <?php print $map['height']; ?>"></div>

<?php if ($map['center'] == TRUE): ?>
  <div id="<?php print $map['map_id']; ?>_center"></div>
<?php endif; ?>

<?php if ($map['zoom'] == TRUE): ?>
  <div id="<?php print $map['map_id']; ?>_zoom"></div>
<?php endif; ?>
