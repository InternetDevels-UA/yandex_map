<?php

/**
 * @file
 * creating blocks for yandex map.
 */


/**
 * Generates an array of maps with a block.
 *
 * @return
 *  An array of map properties indexed by mid.
 *
 * @see yandex_map_block_info()
 * @see yandex_map_block_view()
 */
function yandex_map_blocks() {
  static $blocks = array();
  if ($blocks) {
    return $blocks;
  }
  return $blocks = db_select('yandex_map', 'yandex_map')
    ->fields('yandex_map')
    ->condition('block', 1)
    ->execute()
    ->fetchAllAssoc('mid', PDO::FETCH_ASSOC);
}


/**
 * Implements hook_block_info().
 */
function yandex_map_block_info() {
  $blocks = array();
  $blocks_info = yandex_map_blocks();
  foreach ($blocks_info as $delta => $value) {
    $blocks[$delta] = array(
      'info' => t('Yandex map: @title', array('@title' => $value['title'])),
    );
  }
  
  return $blocks;
}

/**
 * Implements hook_block_view().
 */
function yandex_map_block_view($delta = '') {
  $block = array();
  $blocks_info = yandex_map_blocks();
  if (isset($blocks_info[$delta])) {
    $block['subject'] = $blocks_info[$delta]['title'];
    $block['content'] = yandex_map_view(yandex_map_load($delta));
  }
  return $block;
}