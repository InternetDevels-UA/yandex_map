<?php

/**
 * @file
 * The theme system, which controls the output of yandex map.
 */

/**
 * Template preprocess for yandex-map.tpl.php.
 */
function template_preprocess_yandex_map(&$vars) {
  yandex_map_init_api();

  $vars['map'] = yandex_map_default_map();

  if (isset($vars['map_id'])) {
    $vars['map']['map_id'] = $vars['map_id'];
  }

  // Setup map mode.
  if (!empty($vars['mode'])) {
    $vars['map']['mode'] = $vars['mode'];
  }

  // Add elements to map.
  foreach (array('mode', 'elements', 'type', 'zoom', 'center') as $field) {
    if (!empty($vars[$field])) {
      $vars['map'][$field] = $vars[$field];
    }
  }

  // Send all data to js.
  drupal_add_js(
    array(
      'yandex_map' => array(
        'maps' => array(
          $vars['map']['map_id'] => $vars['map'],
        ),
      ),
    ),
    'setting'
  );
}

/**
 * Template preprocess for view-yandex-map.tpl.php.
 */
function template_preprocess_views_yandex_map(&$vars) {
  yandex_map_init_api();

  $view = $vars['view'];
  $rows = $vars['rows'];

  $vars['classes_array'] = array();
  $vars['classes'] = array();

  // Set up striping values.
  $count = 0;
  $max = count($rows);
  foreach ($rows as $id => $row) {
    $count++;
    $vars['classes'][$id][] = "{$view->name}_{$view->current_display}_" . $view->result[$id]->mid;
    $vars['classes'][$id][] = 'views-row';
    $vars['classes'][$id][] = 'views-row-' . $count;
    $vars['classes'][$id][] = 'views-row-' . ($count % 2 ? 'odd' : 'even');
    if ($count == 1) {
      $vars['classes'][$id][] = 'views-row-first';
    }
    if ($count == $max) {
      $vars['classes'][$id][] = 'views-row-last';
    }

    if ($row_class = $view->style_plugin->get_row_class($id)) {
      $vars['classes'][$id][] = $row_class;
    }

    // Flatten the classes to a string for each row for the template file.
    $vars['classes_array'][$id] = implode(' ', $vars['classes'][$id]);
  }

  $vars['map'] = yandex_map_default_map();
  $vars['map']['map_id'] = "{$view->name}_{$view->current_display}";

  // Views Yandex map style plugin options.
  $views_controls       = array();
  $views_extra_controls = array();
  if (!empty($vars['options'])) {
    $views_controls       = $vars['options']['default_map']['controls'];
    $views_extra_controls = $vars['options']['default_map']['extra_controls'];
  }

  // Configure map controls.
  $controls = yandex_map_controls();
  foreach ($controls as $key => $data) {
    $vars['map']['controls'][$key]['enable'] = !empty($views_controls["yandex_map_controls_{$key}"]) ? TRUE : FALSE;
    foreach (array('top', 'bottom', 'left', 'right') as $pos) {
      if (!empty($views_controls["yandex_map_controls_{$key}_{$pos}"]) && $val = $views_controls["yandex_map_controls_{$key}_{$pos}"]) {
        $vars['map']['controls'][$key][$pos] = (int) $val;
      }
    }
  }

  // Configure map type.
  if ($views_extra_controls['yandex_map_default_type']) {
    $vars['map']['type'] = $views_extra_controls['yandex_map_default_type'];
  }

  // Configure map size.
  if ($views_extra_controls['yandex_map_defalut_height']) {
    $vars['map']['height'] =$views_extra_controls['yandex_map_defalut_height'];
    if (is_numeric($vars['map']['height'])) {
      $vars['map']['height'] .= 'px';
    }
  }
  if ($views_extra_controls['yandex_map_defalut_width']) {
    $vars['map']['width'] = $views_extra_controls['yandex_map_defalut_width'];
    if (is_numeric($vars['map']['width'])) {
      $vars['map']['width'] .= 'px';
    }
  }

  // Configure map behaviors.
  if ($views_extra_controls['yandex_map_defalut_behavior']) {
    $vars['map']['behaviors'] = $views_extra_controls['yandex_map_defalut_behavior'];
  }

  // Configure allowed map types.
  if ($views_extra_controls['yandex_map_enabled_map_types']) {
    $vars['map']['types'] = $views_extra_controls['yandex_map_enabled_map_types'];
  }

  // Preapre geoobjects.
  $vars['map']['elements'] = array();
  foreach (drupal_static('yandex_map_views_elements', array()) as $elements) {
    $vars['map']['elements'] = array_merge($vars['map']['elements'], $elements);
  }

  $vars['map']['mode'] = 'views';

  drupal_add_js(
    array(
      'yandex_map' => array(
        'maps' => array(
          $vars['map']['map_id'] => $vars['map'],
        ),
      ),
    ),
    'setting'
  );
}
