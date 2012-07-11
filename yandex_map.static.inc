<?php

/**
 * @file
 * Contaits static data for Yandex Map configurations.
 */

/**
 * Get avaliable localizations of Ynadex Map interface.
 *
 * @return
 *   A renderable array containing a list of localization keys for Yandex Map
 *   interface.
 */
function yandex_map_localization_list() {
  return array(
    'ru-RU' => t('Russian'),
    'uk-UA' => t('Ukrainian'),
    'en-US' => t('English'),
    'tr-TR' => t('Turkish'),
  );
}

/**
 *
 */
function yandex_map_controls() {
  return array(
    'mapTools' => array(
      'name'          => t('Default map tools'), // стандартный тулбар с кнопками "Навигация", "Выделение" и "Линейка"
      'description'   => t('Maps draggable, magnifier and ruler tools. See more in extra maps controls.'),
      'disabled'      => TRUE,
      'default_value' => TRUE,
    ),
    'editTools' => array(
      'name'          => t('Map edit tools'),
      'disabled'      => TRUE,
      'default_value' => TRUE,
      'description'   => t('Markers, lines and polygons creation tools. See more in extra maps controls.'),
    ),
    'miniMap' => array(
      'name' => t('Mini map'), // обзорная карта
    ),
    'scaleLine' => array(
      'name' => t('Scale line'), // масштабный отрезок
    ),
    'searchControl' => array(
      'name' => t('Search panel'), // панель поиска
    ),
    'typeSelector' => array(
      'name' => t('Map type selector'), // панель переключения типа карты
    ),
    'zoomControl' => array(
      'name' => t('Zoom control'), // ползунок масштаба
    ),
    'smallZoomControl' => array(
      'name' => t('Small zoom control'), // простой ползунок масштаба
    ),
    'trafficControl' => array(
      'name' => t('Traffic control'), // панель пробок
    ),
  );
}

/**
 * Prepare default values for map.
 */
function yandex_map_default_map() {
  static $map_id = 0;

  // Base settings.
  $settings = array(
    'map_id'   => 'yandex_map_' . $map_id,
    'width'    => '100%',
    'height'   => '300px',
    'center'   => '',
    'zoom'     => 11,
    'type'     => 'map',
    'controls' => array(),
  );
  $map_id++;

  // Configure map controls.
  $controls = yandex_map_controls();
  foreach ($controls as $key => $data) {
    $settings['controls'][$key]['enable'] = (bool) variable_get("yandex_map_controls_{$key}", TRUE);
    foreach (array('top', 'bottom', 'left', 'right') as $pos) {
      if ($val = variable_get("yandex_map_controls_{$key}_{$pos}", '')) {
        $settings['controls'][$key][$pos] = (int) $val;
      }
    }
  }

  // Configure map center.
  $settings['center'] = _yandex_map_setting_get('center');
  $settings['zoom'] = _yandex_map_setting_get('zoom');
  $settings['type'] = _yandex_map_setting_get('type');

  // Configure map size.
  $settings['height'] = _yandex_map_setting_get('height');
  if (is_numeric($settings['height'])) {
    $settings['height'] .= 'px';
  }
  $settings['width'] = _yandex_map_setting_get('width');
  if (is_numeric($settings['width'])) {
    $settings['width'] .= 'px';
  }

  // Configure map behaviors.
  $settings['behaviors'] = _yandex_map_setting_get('behaviors');

  // Configure allowed map types.
  $settings['types'] = _yandex_map_setting_get('types');

  // Configure map view and edit tools.
  $settings['mapTools'] = _yandex_map_setting_get('mapTools');
  $settings['editTools'] = _yandex_map_setting_get('editTools');

  // Setup map mode.
  $settings['mode'] = 'view';

  return $settings;
}


/**
 * Help function for default settings.
 */
function _yandex_map_setting_get($key) {
  switch ($key) {
    case 'width':
      return variable_get('yandex_map_default_width', '100%');

    case 'height':
      return variable_get('yandex_map_default_height', 400);

    case 'zoom':
      return  variable_get('yandex_map_default_zoom', 11);

    case 'type':
      return variable_get('yandex_map_default_type', 'map');

    case 'types':
      return variable_get('yandex_map_enabled_map_types', drupal_map_assoc(array_keys(yandex_map_selector_types())));

    case 'center':
      return variable_get('yandex_map_default_center', YANDEX_MAP_DEFAULT_CENTER);

    case 'behavior':
      return variable_get('yandex_map_defalut_behavior', drupal_map_assoc(array('drag', 'scrollZoom', 'dblClickZoom')));

    case 'mapTools':
      return variable_get('yandex_map_mapTools', drupal_map_assoc(array_keys(yandex_map_map_tools())));

    case 'editTools':
      return variable_get('yandex_map_editTools', drupal_map_assoc(array_keys(yandex_map_edit_map_tools())));
  }

  return FALSE;
}


/**
 * Helper function for get all map types.
 */
function yandex_map_selector_types() {
  return array(
    'map'             => t('Map'),
    'satellite'       => t('Satellite'),
    'hybrid'          => t('Hybryd'),
    'publicMap'       => t('Public map'),
    'publicMapHybrid' => t('Public hybryd map'),
  );
}


/**
 * Helper function for get all map behavior types.
 */
function yandex_map_behaviors_types() {
  return array(
    "drag"                      => t('drag'),
    "scrollZoom"                => t('scrollZoom'),
    "dblClickZoom"              => t('dblClickZoom'),
    "rightMouseButtonMagnifier" => t('rightMouseButtonMagnifier'),
    "leftMouseButtonMagnifier"  => t('leftMouseButtonMagnifier'),
    "ruler"                     => t('ruler'),
  );
}


/**
 * Helper function for get all map tools.
 */
function yandex_map_map_tools() {
  return array(
    'drag'      => t('Drag'),
    'magnifier' => t('Magnifier'),
    'ruler'     => t('Ruler'),
  );
}

/**
 * Helper function for get custom edit tools.
 */
function yandex_map_edit_map_tools() {
  return array(
    'placemark' => t('Placemark'),
    'line'      => t('Line'),
    'polygon'   => t('Polygon'),
    'circle'    => t('Circle'),
    'rectangle' => t('Rectangle'),
  );
}
