<?php

/**
 * @file
 * creating fields for yandex map.
 */


/**
 * Implements hook_field_info().
 */
function yandex_map_field_info() {
  return array(
    'geoobjects' => array(
      'label'             => t('Geoobjects'),
      'description'       => t('Yandex geoobject'),
      'default_widget'    => 'no',
      'default_formatter' => 'no',
      //'no_ui'             => TRUE,
    ),
  );
}


/**
 * Implements hook_widget_info().
 */
function yandex_map_field_widget_info() {
  return array(
    'geoobjects' => array(
      'label'       => t('Geoobjects'),
      'description' => '',
      'field types' => array('geoobjects'),
      'behaviors'   => array(
        'multiple values' => FIELD_BEHAVIOR_DEFAULT,
        'default value'   => FIELD_BEHAVIOR_DEFAULT,
      ),
    ),
  );
}


/**
 * Implements hook_field_formatter_info().
 */
function yandex_map_field_formatter_info() {
  return array(
    'list_geoobjects' => array(
      'label'       => t('List of geoobjects'),
      'field types' => array('geoobjects'),
    ),
  );
}

/**
 * Implemetns hook_field_formatter_view().
 */
function yandex_map_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  $settings = $display['settings'];
  switch ($display['type']) {
    case 'list_geoobjects':
      $list = array();
      foreach ($items as $delta => $item) {
        if (!empty($item['description'])) {
          $list_item = $item['description'];
        }
        if (!empty($list_item['icon'])) {
          $list_item .= " {$item['icon']}";
        }
        if (!empty($list_item)) {
          $list[] = $list_item;
        }
      }
      $element[0] = array(
        '#theme' => 'item_list',
        '#items' => $list,
        '#type'  => 'ul',
        'attributes' => array(
          'class' => array('map-geoobjects'),
        ),
      );
      break;
  }

  return $element;
}


/**
 * Implements hook_field_widget_form().
 */
function yandex_map_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  return $element;
}

/**
 * Implements hook_field_is_empty().
 */
function yandex_map_field_is_empty($item, $field) {  
  return !empty($item['coordinates']);
}