<?php

/**
 * @file
 * Contains the list style plugin.
 */

/**
 * Style plugin to render Yandex Map.
 *
 * @ingroup views_style_plugins
 */
class yandex_map_plugin_style_yandex_map extends views_plugin_style {

  // Set default options
  function option_definition() {
    $options = parent::option_definition();

    return $options;
  }

  // Build the settings form for the view.
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    module_load_include('inc', 'yandex_map', 'yandex_map.static');
    module_load_include('inc', 'yandex_map', 'yandex_map.admin');

    yandex_map_settings_build_form($form, $form_state, 'views', $this->options);

    $form['map_id'] = array(
      '#type'  => 'hidden',
      '#value' => "{$form_state['view']->name}_{$form_state['view']->current_display}",
    );
  }

  function validate() {
    $errors = parent::validate();
    if ($this->uses_fields()) {
      $fields = TRUE;
      $field_handlers = $this->view->display_handler->get_handlers('field');
      foreach ($field_handlers as $key => $field) {
        if ($field->real_field == 'geoobjects_description') {
          $fields = FALSE;
          break;
        }
      }
    }
    if ($fields) {
      $errors[] = t(
        "Style '@style' requires a '@field'.",
        array('@style' => $this->definition['title'], '@field' => 'Yandex map: Geoobjects')
      );
    }

    return $errors;
  }
}
