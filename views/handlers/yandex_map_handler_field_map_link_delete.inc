<?php
/**
 * @file
 * Field handler to present a link to delete a map.
 *
 * @ingroup views_field_handlers
 */
class yandex_map_handler_field_map_link_delete extends views_handler_field_node_link {

  /**
   * Renders the link.
   */
  function render_link($entity, $values) {
    $this->options['alter']['make_link'] = TRUE;
    $this->options['alter']['path'] = "yandex-map/{$entity->mid}/delete";
    $this->options['alter']['query'] = drupal_get_destination();

    $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');
    return $text;
  }
}
