<?php

/**
 * @file
 * Contains the yandex map title field handler.
 */

/**
 * Field handler to provide simple renderer that allows linking to a map.
 */
class yandex_map_handler_field_title extends views_handler_field {
  function init(&$view, &$options) {
    parent::init($view, $options);

    if (!empty($this->options['link_to_map'])) {
      $this->additional_fields['mid'] = 'mid';
    }
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['link_to_map'] = array('default' => FALSE);

    return $options;
  }

  /**
   * Provide the link to product option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['link_to_map'] = array(
      '#title'         => t("Link this field to the Yandex map view page"),
      '#description'   => t('This will override any other link you have set.'),
      '#type'          => 'checkbox',
      '#default_value' => !empty($this->options['link_to_map']),
    );
  }

  /**
   * Render whatever the data is as a link to the product.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    if (!empty($this->options['link_to_map']) && $data !== NULL && $data !== '') {
      $mid = $this->get_value($values, 'mid');
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = 'yandex-map/' . $mid;
    }

    return $data;
  }

  function render($values) {
    $value = $this->get_value($values);
    return $this->render_link($this->sanitize_value($value), $values);
  }
}
