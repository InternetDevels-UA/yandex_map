<?php
/**
 * @file
 * Provide node nid argument handler.
 */

/**
 * Argument handler to accept a node id.
 */
class yandex_map_handler_argument_mid extends views_handler_argument_numeric {
  /**
   * Override the behavior of title(). Get the title of the node.
   */
  function title_query() {
    $titles = array();
    $result = db_select('yandex_map', 'ym')
      ->fields('ym', array('title'))
      ->condition('ym.mid', $this->value)
      ->execute();
    foreach ($result as $map) {
      $titles[] = check_plain($map->title);
    }
    return $titles;
  }
}