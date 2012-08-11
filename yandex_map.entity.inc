<?php

/**
 * @file
 * Implements functionality required to create and display an entityes.
 */


/**
 * Implements hook_entity_info()
 */
function yandex_map_entity_info() {
  $info['yandex_map'] = array(
    'label' => t('Yandex map'),
    'controller class' => 'YandexMapController',
    'base table' => 'yandex_map',
    'uri callback' => 'yandex_map_uri',
    'fieldable' => TRUE,
    'entity keys' => array(
      'id' => 'mid' ,
      'label' => 'title',
    ),
    'static cache' => TRUE,
    'bundles' => array(
      'yandex_map' => array(
        'label' => 'Yandex map',
        'admin' => array(
          'path' => 'admin/config/services/yandex/yandex-map',
          'access arguments' => array('administer yandex_map'),
        ),
      ),
    ),
    'view modes' => array(
      'full' => array(
        'label' => t('Full content'),
        'custom settings' =>  FALSE,
      ),
    )
  );

  if (module_exists('search')) {
    $info['yandex_map']['view modes'] += array(
      'search_index' => array(
        'label' => t('Search index'),
        'custom settings' => FALSE,
      ),
      'search_result' => array(
        'label' => t('Search result'),
        'custom settings' => FALSE,
      ),
    );
  }

  $info['yandex_map_marker'] = array(
    'label' => t('Yandex map marker'),
    'controller class' => 'YandexMapMarkerController',
    'base table' => 'yandex_map_marker',
    'fieldable' => TRUE,
    'entity keys' => array(
      'id' => 'mid' ,
      'label' => 'title',
    ),
    'static cache' => TRUE,
    'bundles' => array(
      'yandex_map_marker' => array(
        'label' => 'Yandex map marker',

        'admin' => array(
          'path' => 'admin/config/services/yandex/yandex-map-marker',
          'access arguments' => array('administer yandex_map'),
        ),
      ),
    ),
    'view modes' => array(
      'full' => array(
        'label' => t('Full content'),
        'custom settings' =>  FALSE,
      ),
    )
  );
  return $info;
}


/**
 * YandexMapControllerInterface definition.
 *
 * We create an interface here because anyone could come along and
 * use hook_entity_info_alter() to change our controller class.
 * We want to let them know what methods our class needs in order
 * to function with the rest of the module, so here's a handy list.
 *
 * @see hook_entity_info_alter()
 */

interface YandexMapControllerInterface
  extends DrupalEntityControllerInterface {
  public function create();
  public function save($entity);
  public function delete($entity);
}

/**
 * YandexMapController extends DrupalDefaultEntityController.
 *
 * Our subclass of DrupalDefaultEntityController lets us add a few
 * important create, update, and delete methods.
 */
class YandexMapController
  extends DrupalDefaultEntityController
  implements YandexMapControllerInterface {

  /**
   * Create and return a new entity_example_basic entity.
   */
  public function create() {
    global $user;
    $entity = new stdClass();
    $entity->mid   = 0;
    $entity->uid   = $user->uid;
    $entity->title = '';
    $entity->zoom  = 0;
    $entity->block = 0;
    $entity->center = YANDEX_MAP_DEFAULT_CENTER;
    $entity->created = REQUEST_TIME;
    $entity->changed = REQUEST_TIME;
    return $entity;
  }

  /**
   * Saves the custom fields using drupal_write_record()
   */
  public function save($entity) {
    $entity->changed = REQUEST_TIME;

    // Invoke hook_yandex_map_presave().
    module_invoke_all('yandex_map_presave', $entity);

    // Invoke hook_entity_presave().
    module_invoke_all('entity_presave', $entity, 'yandex_map');
    // The 'primary_keys' argument determines whether this will be an insert
    // or an update. So if the entity already has an ID, we'll specify
    // basic_id as the key.
    $primary_keys = $entity->mid ? 'mid' : array();
    // Write out the entity record.
    drupal_write_record('yandex_map', $entity, $primary_keys);
    // We're going to invoke either hook_entity_update() or
    // hook_entity_insert(), depending on whether or not this is a
    // new entity. We'll just store the name of hook_entity_insert()
    // and change it if we need to.
    $invocation = 'entity_insert';
    // Now we need to either insert or update the fields which are
    // attached to this entity. We use the same primary_keys logic
    // to determine whether to update or insert, and which hook we
    // need to invoke.
    if (empty($primary_keys)) {
      field_attach_insert('yandex_map', $entity);
    }
    else {
      field_attach_update('yandex_map', $entity);
      $invocation = 'entity_update';
    }
    // Invoke either hook_entity_update() or hook_entity_insert().
    module_invoke_all($invocation, $entity, 'yandex_map');
    return $entity;
  }

  /**
   * Delete a single entity.
   *
   * Really a convenience function for delete_multiple().
   */
  public function delete($entity) {
    $this->delete_multiple(array($entity));
  }

  /**
   * Delete one or more yandex_map entities.
   *
   * Deletion is unfortunately not supported in the base
   * DrupalDefaultEntityController class.
   *
   * @param $mids
   *   An array of entity IDs or a single numeric ID.
   */
  public function delete_multiple($entities) {
    $mids = array();
    if (!empty($entities)) {
      $transaction = db_transaction();
      try {
        foreach ($entities as $entity) {

          // Invoke hook_entity_delete().
          module_invoke_all('yandex_map_delete', $entity);

          // Invoke hook_entity_delete().
          module_invoke_all('entity_delete', $entity, 'yandex_map');
          field_attach_delete('yandex_map', $entity);
          $mids[] = $entity->mid;
        }
        db_delete('yandex_map')
          ->condition('mid', $mids, 'IN')
          ->execute();

      }
      catch (Exception $e) {
        $transaction->rollback();
        watchdog_exception('yandex_map', $e);
        throw $e;
      }
    }
  }
}


/**
 * Fetch a Yandex map object.
 *
 * This function ends up being a shim between the menu system and
 * yandex_map_load_multiple().
 *
 * @param $mid
 *   Integer specifying the map entity id.
 * @param $reset
 *   A boolean indicating that the internal cache should be reset.
 * @return
 *   A fully-loaded $map object or FALSE if it cannot be loaded.
 *
 * @see yandex_map_load_multiple()
 * @see yandex_map_menu()
 */
function yandex_map_load($mid = NULL, $reset = FALSE) {
  $mids = (isset($mid) ? array($mid) : array());
  $map = yandex_map_load_multiple($mids, array(), $reset);
  return $map ? reset($map) : FALSE;
}


/**
 * Loads multiple map entities.
 *
 * We only need to pass this request along to entity_load(), which
 * will in turn call the load() method of our entity controller class.
 *
 * @param $mids
 *   An array of map IDs.
 * @param $conditions
 *   (deprecated) An associative array of conditions on the {yandex_map}
 *   table, where the keys are the database fields and the values are the
 *   values those fields must have. Instead, it is preferable to use
 *   EntityFieldQuery to retrieve a list of entity IDs loadable by
 *   this function.
 * @param $reset
 *   Whether to reset the internal yandex_map_load cache.
 *
 * @return
 *   An array of map objects indexed by mid.
 *
 */
function yandex_map_load_multiple($mids = FALSE, $conditions = array(), $reset = FALSE) {
  return entity_load('yandex_map', $mids, $conditions, $reset);
}


/**
 * Implements the uri callback.
 *
 * @param $mids
 *   yandex map object.
 * @return
 *   An array of path settings for map.
 *
 */
function yandex_map_uri($map) {
  return array(
    'path' => 'yandex-map/' . $map->mid,
  );
}


/**
 * Returns a render array with all yandex_map entities.
 */
function yandex_map_list_page($form, &$form_state, $entity = 'yandex_map') {
  if ($entity == 'yandex_map') {
    $path = 'yandex-map';
  }
  else {
    $path = 'yandex-map-marker';
  }
  $header = array(
    'mid'        => array('data' => t('Map id'), 'field' => 'mid'),
    'title'      => array('data' => t('Title'), 'field' => 'title'),
    'created'    => array('data' => t('Created'), 'field' => 'created', 'sort' => 'desc'),
    'operations' => array('data' => t('Operations')),
  );
  $query = db_select($entity, $entity);
  $count_query = clone $query;
  $count_query->addExpression('COUNT(mid)');

  $query
    ->extend('PagerDefault')
    ->extend('TableSort')
    ->fields($entity, array('mid', 'title', 'created'))
    ->limit(20)
    ->orderByHeader($header)
    ->setCountQuery($count_query);
  $result = $query->execute()->fetchAll();

  $destination = drupal_get_destination();

  $options = array();
  foreach ($result as $row) {
    $options[$row->mid] = array(
      'mid'   => $row->mid,
      'title' => $row->title,
      'created'    => date("d-m-Y H:i", $row->created),
      'operations' => array(
        'data' => array(
          '#type'    => 'link',
          '#title'   => t('edit'),
          '#href'    => "{$path}/{$row->mid}/edit",
          '#options' => array('query' => $destination),
        )
      ),
    );
    if ($entity == 'yandex_map') {
     $options[$row->mid]['title'] =  array(
        'data' => array(
          '#type'  => 'link',
          '#title' => $row->title,
          '#href'  => "{$path}/{$row->mid}",
        ),
      );
    }
  }

  $form[$entity] = array(
    '#type'    => 'tableselect',
    '#header'  => $header,
    '#options' => $options,
    '#empty'   => t('No maps available.'),
    '#attributes' => array(),
  );
  $form['delete'] = array(
    '#type'   => 'submit',
    '#value'  => t('Delete'),
    '#submit' => array("{$entity}_edit_delete"),
  );
  $form['#action'] = url('admin/config/services/yandex/delete');
  $form['#method'] = 'get';
  $form['pager']['#theme'] = 'pager';

  return $form;
}


/**
 * Menu callback -- ask for confirmation of items deletion
 */
function yandex_map_multiple_confirm_delete($form, &$form_state) {
  if (!empty($_GET['yandex_map'])) {
    $entity_name = 'yandex_map';
  }
  if (isset($_GET['yandex_map_marker'])) {
    $entity_name = 'yandex_map_marker';
  }
  if (!empty($entity_name)) {
    $form[$entity_name] = array('#type' => 'value', '#value' => $_GET[$entity_name]);
    $form['name'] = array('#type' => 'value', '#value' => $entity_name);
    return confirm_form($form,
      t('Are you sure you want to delete selected items'),
      'admin/config/services/yandex/' . str_replace('_', '-', $entity_name),
      t('This action cannot be undone.'),
      t('Delete'),
      t('Cancel')
    );
  }
  drupal_goto('admin/config/services/yandex/');
}


/**
 * Menu callback -- ask for confirmation of items deletion
 */
function yandex_map_multiple_confirm_delete_submit($form, &$form_state) {
  foreach ($form_state['values'][$form_state['values']['name']] as $mid) {
    if (!empty($mid)) {
      $mids[] = $mid;
    }
  }
  $function = $form_state['values']['name'] . '_load_multiple';
  entity_get_controller($form_state['values']['name'])->delete_multiple($function($mids));
  $form_state['redirect'] = 'admin/config/services/yandex/' . str_replace('_', '-', $form_state['values']['name']);
}


/**
 * Callback for a page title when this entity is displayed.
 */
function yandex_map_title($entity) {
  return $entity->title;
}


/**
 * Function to display the entity.
 */
function yandex_map_view($entity, $view_mode = 'basic') {
  module_invoke_all('yandex_map_view', $entity);
  yandex_map_build_content($entity, $view_mode);

  $build = array(
    '#theme'       => 'yandex_map',

    '#map_id'      => 'view_' . $entity->mid,
    '#mode'        => 'view',
    '#elements'    => yandex_map_entity_to_geoobjects($entity),

    '#zoom'        => $entity->zoom,
    '#type'        => $entity->type,
    '#center'      => $entity->center,

    '#show_center' => FALSE,
    '#show_zoom'   => FALSE,

    '#content'     => $entity->content,
  );

  // We don't need duplicate rendering info in yandex_map->content.
  unset($entity->content);

  // Allow modules to modify the structured node.
  $type = 'yandex_map';
  drupal_alter(array('yandex_map_view', 'entity_view'), $build, $type);

  return $build;
}


/**
 * Provides a wrapper on the edit form to add a new entity.
 */
function yandex_map_add() {
  // Create a basic entity structure to be used and passed to the validation
  // and submission functions.
  $entity = entity_get_controller('yandex_map')->create();
  return drupal_get_form('yandex_map_form', $entity);
}


/**
 * Form function to create an yandex_map entity.
 *
 * The pattern is:
 * - Set up the form for the data that is specific to your
 *   entity: the columns of your base table.
 * - Call on the Field API to pull in the form elements
 *   for fields attached to the entity.
 */
function yandex_map_form($form, &$form_state, $entity) {
  $form['title'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Title'),
    '#required'      => TRUE,
    '#default_value' => $entity->title,
  );

  $form['block'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Create block for this map'),
    '#default_value' => $entity->block,
  );

  $form['map_preview'] = array(
    '#theme'       => 'yandex_map',

    '#map_id'      => 'edit_' . $entity->mid,
    '#mode'        => 'edit',
    '#elements'    => yandex_map_entity_to_geoobjects($entity),
  );
  if (!empty($entity->type)) {
    $form['map_preview']['#type'] = $entity->type;
  }
  if (!empty($entity->zoom)) {
    $form['map_preview']['#zoom'] = $entity->zoom;
  }
  if (!empty($entity->center)) {
    $form['map_preview']['#center'] = $entity->center;
  }

  $form['settings'] = array(
    '#type'       => 'textarea',
    '#attributes' => array(
      'id' => 'yandex_map_' . $entity->mid . '_settings',
    ),
    '#prefix' => '<div style="display:none">',
    '#suffix' => '</div>',
  );
  $form['yandex_map'] = array(
    '#type'  => 'value',
    '#value' => $entity,
  );

  field_attach_form('yandex_map', $entity, $form, $form_state);

  $form['submit'] = array(
    '#type'   => 'submit',
    '#value'  => t('Save'),
    '#weight' => 100,
  );
  if ($entity->mid) {
    $form['delete'] = array(
      '#type'   => 'submit',
      '#value'  => t('Delete'),
      '#submit' => array('yandex_map_edit_delete'),
      '#weight' => 200,
    );
  }
  return $form;
}


/**
 * Validation handler for yandex_map_form form.
 * We pass things straight through to the Field API to handle validation
 * of the attached fields.
 */
function yandex_map_form_validate($form, &$form_state) {
  field_attach_form_validate('yandex_map', $form_state['values']['yandex_map'], $form, $form_state);
}


/**
 * Form submit handler: submits yandex_map_form information
 */
function yandex_map_form_submit($form, &$form_state) {
  $map = $form_state['values']['yandex_map'];
  $map->title = $form_state['values']['title'];
  $map->block = $form_state['values']['block'];
  field_attach_submit('yandex_map', $map, $form, $form_state);
  $map = yandex_map_json_decode($form_state['values']['settings'], $map);
  $insert = empty($map->mid);
  $map = yandex_map_save($map);
  $map_link = l(t('view'), 'yandex-map/' . $map->mid);
  $message_args = array('%title' => $map->title);
  if ($insert) {
    watchdog('yandex_map', 'Map added %title.', $message_args, WATCHDOG_NOTICE, $map_link);
    drupal_set_message(t('Map %title has been created.', $message_args));
  }
  else {
    watchdog('yandex_map', 'Map: updated %title.', $message_args, WATCHDOG_NOTICE, $map_link);
    drupal_set_message(t('Map %title has been updated.', $message_args));
  }
  if (!empty($map->mid)) {
    $form_state['redirect'] = 'yandex-map/' . $map->mid;
  }
  else {
    // In the unlikely case something went wrong on save, the map will be
    // rebuilt and map form redisplayed.
    drupal_set_message(t('The map could not be saved.'), 'error');
    $form_state['rebuild'] = TRUE;
  }
}


/**
 * Form deletion handler.
 *
 * @todo: 'Are you sure?' message.
 */
function yandex_map_edit_delete($form, &$form_state) {
  if (!empty($form_state['values']['yandex_map'])) {
    $destination = array();
    if (isset($_GET['destination'])) {
      $destination = drupal_get_destination();
      unset($_GET['destination']);
    }
    $form_state['redirect'] = array('yandex-map/' . $form_state['values']['yandex_map']->mid . '/delete', array('query' => $destination));
  }
}


/**
 * Menu callback -- ask for confirmation of map deletion
 */
function yandex_map_delete_form($form, &$form_state, $map) {
  $form['#map'] = $map;
  $form['yandex_map'] = array('#type' => 'value', '#value' => $map);
  return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $map->title)),
    'admin/config/services/yandex/yandex-map',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}


/**
 * Execute map deletion
 */
function yandex_map_delete_form_submit($form, &$form_state) {
  if (isset($form_state['values']['confirm'])) {
    $map = $form_state['values']['yandex_map'];
    yandex_map_delete($map);
    drupal_set_message(t('The map %item_description (ID %id) has been deleted',
      array('%item_description' => $map->title, '%id' => $map->mid))
    );
  }
  $form_state['redirect'] = 'admin/config/services/yandex/yandex-map';
}


/**
 * We save the entity by calling the controller.
 */
function yandex_map_save(&$entity) {
  return entity_get_controller('yandex_map')->save($entity);
}


/**
 * Use the controller to delete the entity.
 */
function yandex_map_delete($entity) {
  entity_get_controller('yandex_map')->delete($entity);
}


/**
 * YandexMapMarkerController extends DrupalDefaultEntityController.
 *
 * Our subclass of DrupalDefaultEntityController lets us add a few
 * important create, update, and delete methods.
 */
class YandexMapMarkerController
  extends DrupalDefaultEntityController
  implements YandexMapControllerInterface {

  /**
   * Create and return a new entity_example_basic entity.
   */
  public function create() {
    global $user;
    $entity = new stdClass();
    $entity->mid   = 0;
    $entity->uid   = $user->uid;
    $entity->title = '';
    $entity->created = REQUEST_TIME;
    $entity->changed = REQUEST_TIME;
    return $entity;
  }

  /**
   * Saves the custom fields using drupal_write_record()
   */
  public function save($entity) {
    $entity->changed = REQUEST_TIME;

    // Invoke hook_yandex_map_presave().
    module_invoke_all('yandex_map_marker_presave', $entity);

    // Invoke hook_entity_presave().
    module_invoke_all('entity_presave', $entity, 'yandex_map_marker');
    // The 'primary_keys' argument determines whether this will be an insert
    // or an update. So if the entity already has an ID, we'll specify
    // basic_id as the key.
    $primary_keys = $entity->mid ? 'mid' : array();
    // Write out the entity record.
    drupal_write_record('yandex_map_marker', $entity, $primary_keys);
    // We're going to invoke either hook_entity_update() or
    // hook_entity_insert(), depending on whether or not this is a
    // new entity. We'll just store the name of hook_entity_insert()
    // and change it if we need to.
    $invocation = 'entity_insert';
    // Now we need to either insert or update the fields which are
    // attached to this entity. We use the same primary_keys logic
    // to determine whether to update or insert, and which hook we
    // need to invoke.
    if (empty($primary_keys)) {
      field_attach_insert('yandex_map_marker', $entity);
    }
    else {
      field_attach_update('yandex_map_marker', $entity);
      $invocation = 'entity_update';
    }
    // Invoke either hook_entity_update() or hook_entity_insert().
    module_invoke_all($invocation, $entity, 'yandex_map_marker');
    return $entity;
  }

  /**
   * Delete a single entity.
   *
   * Really a convenience function for delete_multiple().
   */
  public function delete($entity) {
    $this->delete_multiple(array($entity));
  }

  /**
   * Delete one or more yandex_map entities.
   *
   * Deletion is unfortunately not supported in the base
   * DrupalDefaultEntityController class.
   *
   * @param $mids
   *   An array of entity IDs or a single numeric ID.
   */
  public function delete_multiple($entities) {
    $mids = array();
    if (!empty($entities)) {
      $transaction = db_transaction();
      try {
        foreach ($entities as $entity) {

          // Invoke hook_entity_delete().
          module_invoke_all('yandex_map_marker_delete', $entity);

          // Invoke hook_entity_delete().
          module_invoke_all('entity_delete', $entity, 'yandex_map_marker');
          field_attach_delete('yandex_map_marker', $entity);
          $mids[] = $entity->mid;
        }
        db_delete('yandex_map_marker')
          ->condition('mid', $mids, 'IN')
          ->execute();

      }
      catch (Exception $e) {
        $transaction->rollback();
        watchdog_exception('yandex_map_marker', $e);
        throw $e;
      }
    }
  }
}


/**
 * Fetch a Yandex map object.
 *
 * @see yandex_map_marker_load_multiple()
 * @see yandex_map_menu()
 */
function yandex_map_marker_load($mid = NULL, $reset = FALSE) {
  $mids = (isset($mid) ? array($mid) : array());
  $map = yandex_map_marker_load_multiple($mids, array(), $reset);
  return $map ? reset($map) : FALSE;
}


/**
 * Loads multiple marker entities.
 */
function yandex_map_marker_load_multiple($mids = FALSE, $conditions = array(), $reset = FALSE) {
  return entity_load('yandex_map_marker', $mids, $conditions, $reset);
}


/**
 * Provides a wrapper on the edit form to add a new entity.
 */
function yandex_map_marker_add() {
  $entity = entity_get_controller('yandex_map_marker')->create();
  return drupal_get_form('yandex_map_marker_form', $entity);
}


/**
 * Form function to create an yandex_map_marker entity.
 */
function yandex_map_marker_form($form, &$form_state, $marker) {
  $form['title'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Title'),
    '#required'      => TRUE,
    '#default_value' => $marker->title,
  );

  field_attach_form('yandex_map_marker', $marker, $form, $form_state);

  $form['yandex_map_marker'] = array(
    '#type'  => 'value',
    '#value' => $marker,
  );
  $form['submit'] = array(
    '#type'   => 'submit',
    '#value'  => t('Save'),
    '#weight' => 100,
  );
  if ($marker->mid) {
    $form['delete'] = array(
      '#type' => 'submit',
      '#value'  => t('Delete'),
      '#submit' => array('yandex_map_marker_edit_delete'),
      '#weight' => 200,
    );
  }
  return $form;
}


/**
 * Validation handler for yandex_map_marker_form form.
 */
function yandex_map_marker_form_validate($form, &$form_state) {
  field_attach_form_validate('yandex_map_marker', $form_state['values']['yandex_map_marker'], $form, $form_state);
}


/**
 * Form submit handler: submits yandex_map_marker_form information
 */
function yandex_map_marker_form_submit($form, &$form_state) {
  $marker = $form_state['values']['yandex_map_marker'];
  $marker->title = $form_state['values']['title'];
  field_attach_submit('yandex_map_marker', $marker, $form, $form_state);
  $insert = empty($map->mid);
  $marker = yandex_map_marker_save($marker);
  $message_args = array('%title' => $marker->title);
  if ($insert) {
    drupal_set_message(t('Marker %title has been created.', $message_args));
  }
  else {
    drupal_set_message(t('Marker %title has been updated.', $message_args));
  }
  if (!empty($marker->mid)) {
    $form_state['redirect'] = 'admin/config/services/yandex/yandex-map-marker';
  }
  else {
    // In the unlikely case something went wrong on save, the marker will be
    // rebuilt and map form redisplayed.
    drupal_set_message(t('The marker could not be saved.'), 'error');
    $form_state['rebuild'] = TRUE;
  }
}


/**
 * Form deletion handler.
 *
 * @todo: 'Are you sure?' message.
 */
function yandex_map_marker_edit_delete($form , &$form_state ) {
  if (!empty($form_state['values']['yandex_map_marker'])) {
    $destination = array();
    if (isset($_GET['destination'])) {
      $destination = drupal_get_destination();
      unset($_GET['destination']);
    }
    $form_state['redirect'] = 'yandex-map-marker/' . $form_state['values']['yandex_map_marker']->mid . '/delete';
  }
}


/**
 * Menu callback -- ask for confirmation of marker deletion
 */
function yandex_map_marker_delete_form($form, &$form_state, $marker) {
  $form['#marker'] = $marker;
  $form['yandex_map_marker'] = array('#type' => 'value', '#value' => $marker);
  return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $marker->title)),
    'admin/config/services/yandex/yandex-map',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}


/**
 * Execute map deletion
 */
function yandex_map_marker_delete_form_submit($form, &$form_state) {
  if (isset($form_state['values']['confirm'])) {
    $marker = $form_state['values']['yandex_map_marker'];
    yandex_map_marker_delete($marker);
    drupal_set_message(t('The map %item_description (ID %id) has been deleted',
      array('%item_description' => $marker->title, '%id' => $marker->mid))
    );
  }
  $form_state['redirect'] = 'admin/config/services/yandex/yandex-map-marker';
}


/**
 * We save the entity by calling the controller.
 */
function yandex_map_marker_save(&$entity) {
  return entity_get_controller('yandex_map_marker')->save($entity);
}


/**
 * Use the controller to delete the entity.
 */
function yandex_map_marker_delete($entity) {
  entity_get_controller('yandex_map_marker')->delete($entity);
}
