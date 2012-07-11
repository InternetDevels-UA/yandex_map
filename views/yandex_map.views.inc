<?php

/**
 * @file
 * Defines the View Style Plugins for Yandex Map module.
 */

/**
 * Implements hook_views_plugins().
 */
function yandex_map_views_plugins() {
  return array(
    'style' => array(
      'yandex_map' => array(
        'title' => t('Yandex Map'),
        'help' => t('Views format for Yandex map.'),
        'handler' => 'yandex_map_plugin_style_yandex_map',
        'uses options' => TRUE,
        'uses row plugin' => TRUE,
        'uses grouping' => FALSE,
        'uses row class' => TRUE,
        'type' => 'normal',
        'path' => drupal_get_path('module', 'yandex_map') . '/views/plugins',
        'theme' => 'views_yandex_map',
        'theme path' => drupal_get_path('module', 'yandex_map') . '/theme',
        'theme file' => 'yandex_map.theme.inc',
      ),
    ),
  );
}

/**
 * Implements hook_views_data().
 */
function yandex_map_views_data() {
  $data = array();

  $data['yandex_map']['table']['group']  = t('Yandex Map');
  $data['yandex_map']['table']['base'] = array(
    'field' => 'mid',
    'title' => t('Yandex Map'),
    'help' => t('Yandex Map.'),
  );
  $data['yandex_map']['table']['entity type'] = 'yandex_map';

  // Expose the Map ID.
  $data['yandex_map']['mid'] = array(
    'title' => t('Map ID'),
    'help' => t('The unique internal identifier of the Yandex map.'),
    'field' => array(
      'handler' => 'yandex_map_handler_field_title',
      'click sortable' => TRUE,
      'path' => drupal_get_path('module', 'yandex_map') . '/views/handlers',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'yandex_map_handler_argument_mid',
      'path' => drupal_get_path('module', 'yandex_map') . '/views/handlers',
    ),
  );

  // Expose the product title.
  $data['yandex_map']['title'] = array(
    'title' => t('Title'),
    'help' => t('The title of the yandex map.'),
    'field' => array(
      'handler' => 'yandex_map_handler_field_title',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Created field.
  $data['yandex_map']['created'] = array(
    'title' => t('Post date'), // The item it appears as on the UI,
    'help' => t('The date the map was created.'), // The help that appears on the UI,
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // Changed field.
  $data['yandex_map']['changed'] = array(
    'title' => t('Updated date'), // The item it appears as on the UI,
    'help' => t('The date the map was last updated.'), // The help that appears on the UI,
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // Expose the creator uid.
  $data['yandex_map']['uid'] = array(
    'title' => t('Creator'),
    'help' => t('Relate a map to the user who created it.'),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'field' => 'uid',
      'label' => t('Map creator'),
    ),
  );

  // View map link field.
  $data['yandex_map']['view_map'] = array(
    'field' => array(
      'title' => t('Link'),
      'help' => t('Provide a simple link to the map.'),
      'handler' => 'yandex_map_handler_field_map_link',
    ),
  );

  // Edit map link field.
  $data['yandex_map']['edit_map'] = array(
    'field' => array(
      'title' => t('Edit link'),
      'help' => t('Provide a simple link to edit the map.'),
      'handler' => 'yandex_map_handler_field_map_link_edit',
    ),
  );

  // Delete map link field.
  $data['yandex_map']['delete_map'] = array(
    'field' => array(
      'title' => t('Delete link'),
      'help' => t('Provide a simple link to delete the map.'),
      'handler' => 'yandex_map_handler_field_map_link_delete',
    ),
  );

  return $data;
}

/**
 * Implements hook_field_views_data().
 *
 * @see field_views_field_default_views_data()
 */
function yandex_map_field_views_data($field) {
  $data = field_views_field_default_views_data($field);

  foreach ($data as $table_name => $table_data) {
    foreach ($table_data as $field_name => $field_data) {
      if (isset($data[$table_name][$field_name]['field'])) {
        if ($field_name == 'geoobjects') {
          $data[$table_name][$field_name]['field']['handler'] = 'yandex_map_handler_field_field';
          $data[$table_name][$field_name]['field']['path']    = drupal_get_path('module', 'yandex_map') . '/views/handlers';
        }
      }
    }
  }

  return $data;
}
