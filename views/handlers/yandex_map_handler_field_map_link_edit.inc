<?php
/**
 * @file
 * Field handler to present a link map edit.
 *
 * @ingroup views_field_handlers
 */
class yandex_map_handler_field_map_link_edit extends yandex_map_handler_field_map_link {

  /**
   * Renders the link.
   */
  function render_link($entity, $values) {
    $this->options['alter']['make_link'] = TRUE;
    $this->options['alter']['path'] = "yandex-map/{$entity->mid}/edit";
    $this->options['alter']['query'] = drupal_get_destination();

    $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');
    return $text;
  }
}
