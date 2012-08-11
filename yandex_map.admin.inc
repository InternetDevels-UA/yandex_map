<?php

/**
 * @file
 * Yandex Map settings form.
 */

/**
 * Admin settings form.
 */
function yandex_map_settings($form, &$form_state) {
  yandex_map_settings_build_form($form, $form_state, 'admin');

  return system_settings_form($form);
}

/**
 * Build Yandex map settings form.
 * @param
 *   Type of settings form. One of the following values are possible:
 *   - 'admin' - form of yandex map administration page
 *   - 'views' - form of yandex map views plugin
 */
function yandex_map_settings_build_form(&$form, &$form_state, $type, $options = array()) {
  // Yandex API key.
  if ($type == 'admin') {
    $form['yandex_map_api_key'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Yandex API Key'),
      '#description'   => t(
        'Your personal Yandex Map API key. If you want to use an API key, you can get one at !link. But API key isn\'t required (!documentation).',
        array(
          '!link'          => l(t('Yandex Map API key form'), 'http://api.yandex.ru/maps/form.xml'),
          '!documentation' => l(t('read documentation'), 'http://api.yandex.ru/maps/doc/jsapi/2.x/update/concepts/update.xml'),
        )
      ),
      '#default_value' => variable_get('yandex_map_api_key', ''),
      '#size'          => 50,
      '#maxlength'     => 255,
    );

    // Yandex Map interface language.
    $form['yandex_map_interface_language'] = array(
      '#type'          => 'select',
      '#title'         => t('Language'),
      '#description'   => t('This will set the language used for the interface of map.'),
      '#options'       => yandex_map_localization_list(),
      '#default_value' => variable_get('yandex_map_interface_language', 'ru-RU'),
    );
  }

  if ($type == 'admin') {
    // Yandex map settings.
    $form['default_map'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('Default map settings'),
      '#collapsible' => TRUE,
      '#collapsed'   => FALSE,
    );
    // Map preview.
    $form['default_map']['yandex_map_preview'] = array(
      '#theme'  => 'yandex_map',
      '#map_id' => 'admin_preview',
      '#mode'   => 'edit'
    );

    // Default map center.
    $form['default_map']['yandex_map_default_center'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Default map center'),
      '#description'   => t('The default center coordinates, expressed as a decimal latitude and longitude, separated by a comma.'),
      '#default_value' => variable_get('yandex_map_default_center', YANDEX_MAP_DEFAULT_CENTER),
      '#prefix'        => '<div style="display:none">',
      '#suffix'        => '</div>',
    );

    // Default map zoom.
    $form['default_map']['yandex_map_default_zoom'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Default map zoom'),
      '#description'   => t(''),
      '#default_value' => variable_get('yandex_map_default_zoom', ''),
      '#prefix'        => '<div style="display:none">',
      '#suffix'        => '</div>',
    );
  }

  // Map tools settings.
  $map_settings = yandex_map_controls();
  $form['default_map']['controls'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Map controls'),
    '#description' => t("Settings changes will be applied immediately and can be seen on map preview. This settings aren't required and if they aren't setted then default settings will be used."),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
    '#theme'       => 'yandex_map_controls_table',
  );
  foreach ($map_settings as $key => $data) {
    $default_value       = (isset($data['default_value'])) ? $data['default_value'] : TRUE;
    $control_description = isset($data['description']) ? $data['description'] : '';
    $form['default_map']['controls']["yandex_map_controls_{$key}"] = array(
      '#type'          => 'checkbox',
      '#title'         => check_plain($data['name']),
      '#description'   => check_plain($control_description),
      '#disabled'      => (isset($data['disabled'])) ? $data['disabled'] : FALSE,
    );

    // Set default values depends on form $type.
    $field_key = "yandex_map_controls_{$key}";
    switch ($type) {
      case 'views':
        $form['default_map']['controls'][$field_key]['#default_value'] = $default_value;
        if (!empty($options['default_map']['controls']["yandex_map_controls_{$key}"])) {
          $form['default_map']['controls'][$field_key]['#default_value'] = $options['default_map']['controls'][$field_key];
        }
        break;

      default:
        $form['default_map']['controls'][$field_key]['#default_value'] = variable_get($field_key, $default_value);
        break;
    }

    if (!isset($data['position']) || $data['position'] == TRUE) {
      // Position of tool.
      $pos_array = array(
        'top' => array(
          'invisible' => 'bottom',
        ),
        'bottom' => array(
          'invisible' => 'top',
        ),
        'left' => array(
          'invisible' => 'right',
        ),
        'right' => array(
          'invisible' => 'left',
        ),
      );
      foreach ($pos_array as $pos => $pos_data) {
        $form['default_map']['controls']["yandex_map_controls_{$key}_{$pos}"] = array(
          '#type'             => 'textfield',
          '#size'             => 4,
          '#field_suffix'     => t('px'),
          '#element_validate' => array('element_validate_integer'),
        );
        // Set default values and states depends on form $type.
        $field_key = "yandex_map_controls_{$key}_{$pos}";
        switch ($type) {
          case 'views':
            $input     = "style_options[default_map][controls][yandex_map_controls_{$key}]";
            $input_pos = "style_options[default_map][controls][yandex_map_controls_{$key}_{$pos_data['invisible']}]";
            $form['default_map']['controls'][$field_key]['#default_value'] = $options['default_map']['controls'][$field_key];
            break;

          default:
            $input     = "yandex_map_controls_{$key}";
            $input_pos = "yandex_map_controls_{$key}_{$pos_data['invisible']}";
            $form['default_map']['controls'][$field_key]['#default_value'] = variable_get($field_key, '');
            break;
        }
        $form['default_map']['controls'][$field_key]['#states'] = array(
          'visible' => array(
            ":input[name='{$input}']"     => array('checked' => TRUE),
            ":input[name='{$input_pos}']" => array('filled' => FALSE),
          ),
        );
      }
    }
  }

  // Extra map controls.
  $form['default_map']['extra_controls'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Extra map controls'),
    '#description' => t('Settings changes will be applied after saving configuration.'),
    '#collapsible' => TRUE,
    '#collapsed'   => TRUE,
  );

  // Map width.
  $form['default_map']['extra_controls']['yandex_map_default_width'] = array(
    '#type'        => 'textfield',
    '#title'       => t('Map width'),
    '#description' => t('Map width can be setted in pixels or in percents.'),
    '#size'        => 6,
  );

  // Map height.
  $form['default_map']['extra_controls']['yandex_map_default_height'] = array(
    '#type'             => 'textfield',
    '#title'            => t('Map height'),
    '#description'      => t('Map height must be an interget value.'),
    '#size'             => 6,
    '#element_validate' => array('element_validate_integer'),
  );

  // Map behaviour.
  $form['default_map']['extra_controls']['yandex_map_default_behavior'] = array(
    '#type'    => 'checkboxes',
    '#title'   => t('Map behavior types'),
    '#options' => yandex_map_behaviors_types(),
  );

  // Enabled map types.
  $form['default_map']['extra_controls']['yandex_map_enabled_map_types'] = array(
    '#type'        => 'checkboxes',
    '#title'       => t('Enabled map types'),
    '#description' => t('Select map types which will be available in Yandex Map type selector.'),
    '#options'     => yandex_map_selector_types(),
    '#ajax'        => array(
      'wrapper'  => 'default-map-type',
      'callback' => 'yandex_map_default_map_type_ajax_callback',
    ),
  );

  // Default map selector type.
  if (isset($form_state['values']) && !empty($form_state['values']['yandex_map_enabled_map_types'])) {
    $map_types_selected = $form_state['values']['yandex_map_enabled_map_types'];
  }
  else {
    $map_types_selected = variable_get('yandex_map_enabled_map_types', '');
  }
  $form['default_map']['extra_controls']['yandex_map_default_type'] = array(
    '#type'          => 'select',
    '#title'         => t('Default map type'),
    '#description'   => t(''),
    '#options'       => yandex_map_default_type_options($map_types_selected),
    '#prefix'        => '<div id="default-map-type">',
    '#suffix'        => '</div>',
  );

  // @TODO: Search line weigth.
  //$form['default_map']['extra_controls']['yandex_map_searchControl_width'] = array(
  //  '#type'             => 'textfield',
  //  '#title'            => t('Search line width'),
  //  '#description'      => t(''),
  //  '#size'             => 6,
  //  '#element_validate' => array('element_validate_integer'),
  //);

  // @TODO: Minimap zoomOffset setting.
  // @TODO: Minimap Minimap size setting.
  // @TODO: Minimap expanded setting.

  // @TODO: Map tools settings.
  //$form['default_map']['extra_controls']['yandex_map_mapTools'] = array(
  //  '#type'          => 'checkboxes',
  //  '#title'         => t('Map tools'),
  //  '#description'   => t('This tools will appear on map viewing and editing.'),
  //  '#options'       => yandex_map_map_tools(),
  //  '#required'      => TRUE,
  //);

  // Additional map tools settings.
  $form['default_map']['extra_controls']['yandex_map_editTools'] = array(
    '#type'          => 'checkboxes',
    '#title'         => t('Edit map tools'),
    '#description'   => t('This tools will appear on map editing.'),
    '#options'       => yandex_map_edit_map_tools(),
  );

  // Set default values and states.
  $fields = array(
    'yandex_map_default_width'     => _yandex_map_setting_get('width'),
    'yandex_map_default_height'    => _yandex_map_setting_get('height'),
    'yandex_map_default_behavior'  => _yandex_map_setting_get('behavior'),
    'yandex_map_enabled_map_types' => _yandex_map_setting_get('types'),
    'yandex_map_default_type'      => _yandex_map_setting_get('type'),
    'yandex_map_editTools'         => _yandex_map_setting_get('editTools'),
  );
  switch ($type) {
    case 'views':
      foreach ($fields as $key => $data) {
        $form['default_map']['extra_controls'][$key]['#default_value'] = $data;
        if (!empty($options['default_map']['extra_controls'][$key])) {
          $form['default_map']['extra_controls'][$key]['#default_value'] = $options['default_map']['extra_controls'][$key];
        }
      }
      $input = 'style_options[default_map][controls][yandex_map_controls_typeSelector]';
      break;

    default:
      foreach ($fields as $key => $data) {
        $form['default_map']['extra_controls'][$key]['#default_value'] = $data;
      }
      $input = 'yandex_map_controls_typeSelector';
      break;
  }

  // Enabled map types.
  $form['default_map']['extra_controls']['yandex_map_enabled_map_types']['#states'] = array(
    'visible' => array(
      ":input[name='{$input}']" => array('checked' => TRUE),
    ),
  );
}

/**
 *
 */
function yandex_map_default_map_type_ajax_callback($form, $form_state) {
  return $form['default_map']['extra_controls']['yandex_map_default_type'];
}

/**
 *
 */
function yandex_map_default_type_options($selected) {
  $map_types      = yandex_map_selector_types();
  $options        = array();
  if (!empty($selected)) {
    foreach ($selected as $value) {
      if ($value) {
        $options[$value] = $map_types[$value];
      }
    }
  }
  if (empty($options)) {
    $options = $map_types;
  }

  return $options;
}

/**
 * Render Yandex Map controls settings as table.
 */
function theme_yandex_map_controls_table($variables) {
  $element = $variables['element'];
  $header = array(
    '',
    t('Top'),
    t('Bottom'),
    t('Left'),
    t('Right'),
  );

  $rows = array();
  $controls = yandex_map_controls();
  foreach ($controls as $key => $data) {
    if (isset($element["yandex_map_controls_{$key}"])) {
      $row[] = drupal_render($element["yandex_map_controls_{$key}"]);
      foreach (array('top', 'bottom', 'left', 'right') as $pos) {
        $row[] = drupal_render($element["yandex_map_controls_{$key}_{$pos}"]);
      }
      $rows[] = $row;
      $row    = array();
    }
  }
  $table = array(
    '#theme'  => 'table',
    '#header' => $header,
    '#rows'   => $rows,
  );
  return drupal_render($table) . drupal_render_children($element);
}
