<?php

/**
 * @file
 * Contains the basic yandex map geoobject field handler.
 */

/**
 *
 */
class yandex_map_handler_field_field extends views_handler_field_field {
  function init(&$view, &$options) {
    parent::init($view, $options);
  }

  function option_definition() {
    $options = parent::option_definition();
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // Set exclude by default.
    //$form['exclude']['#default_value'] = TRUE;
    //hide($form['exclude']);

    $form['custom_label']['#default_value'] = FALSE;
    hide($form['custom_label']);
    $form['element_label_colon'] = FALSE;
    hide($form['element_label_colon']);
    hide($form['label']);
    hide($form['type']);
  }

  function get_value($values, $field = NULL) {
    parent::get_value($values, $field);

    $view = $this->view;
    $style_options = $view->display[$view->current_display]->handler->get_option('style_options');

    $all = &drupal_static('yandex_map_views_elements', array());
    $entity = $values->_field_data[$this->field_alias]['entity'];
    $all[$entity->mid] = yandex_map_entity_to_geoobjects($entity);
  }
}
