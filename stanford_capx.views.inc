<?php
/**
 * @file
 * Expose and change data for views.
 */


/**
 * Exposed the configuration entity serialized
 * @return [type] [description]
 */
function stanford_capx_views_data() {
  $data = array();

   // First, the entry $data['example_table']['table'] describes properties of
  // the actual table – not its content.

  // The 'group' index will be used as a prefix in the UI for any of this
  // table's fields, sort criteria, etc. so it's easy to tell where they came
  // from.
  $data['capx_cfe']['table']['group'] = t('Configuration Entity');

  // Define this as a base table – a table that can be described in itself by
  // views (and not just being brought in as a relationship). In reality this
  // is not very useful for this table, as it isn't really a distinct object of
  // its own, but it makes a good example.
  $data['capx_cfe']['table']['base'] = array(
    'field' => 'cfid', // This is the identifier field for the view.
    'title' => t('CAPx Configuration Entity'),
    'help' => t('CAPx Configuration Entity contains the settings for the mapper and importer entities.'),
    'weight' => -10,
  );

  $data['capx_cfe']['table']['entity type'] = 'capx_cfe';

  // settings table field.
  $data['capx_cfe']['settings'] = array(
    'title' => t('Configuration Settings'),
    'help' => t('Exposes the serialized array of settings.'),
    'field' => array(
      'handler' => 'views_handler_field_serialized',
      'click sortable' => FALSE, // This is use by the table display plugin.
    ),
  );

  // settings table field.
  $data['capx_cfe']['meta'] = array(
    'title' => t('Meta Data'),
    'help' => t('Exposes meta data.'),
    'field' => array(
      'handler' => 'views_handler_field_serialized',
      'click sortable' => FALSE, // This is use by the table display plugin.
    ),
  );

  // Example plain text field.
  $data['capx_cfe']['machine_name'] = array(
    'title' => t('Machine name'),
    'help' => t('The machine name of the entity.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Example numeric text field.
  $data['capx_cfe']['cfid'] = array(
    'title' => t('Configuration ID'),
    'help' => t('The ID of the configuration entity.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Example plain text field.
  $data['capx_cfe']['title'] = array(
    'title' => t('Title'),
    'help' => t('The human readable title of the entity.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Example plain text field.
  $data['capx_cfe']['type'] = array(
    'title' => t('type'),
    'help' => t('The type of entity.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

 // uid field
  $data['capx_cfe']['uid'] = array(
    'title' => t('Author uid'),
    'help' => t('The user authoring the content. If you need more fields than the uid add the content: author relationship'),
    'relationship' => array(
      'title' => t('Author'),
      'help' => t('Relate content to the user who created it.'),
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'field' => 'uid',
      'label' => t('author'),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_user_name',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'field' => array(
      'handler' => 'views_handler_field_user',
    ),
  );

  // Example timestamp field.
  $data['capx_cfe']['created'] = array(
    'title' => t('Created'),
    'help' => t('The date the entity was created.'),
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

  // Example timestamp field.
  $data['capx_cfe']['changed'] = array(
    'title' => t('Changed'),
    'help' => t('The date the entity was last changed.'),
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

  // CAPx PROFILES
  // ---------------------------------------------------------------------------


  // The 'group' index will be used as a prefix in the UI for any of this
  // table's fields, sort criteria, etc. so it's easy to tell where they came
  // from.
  $data['capx_profiles']['table']['group'] = t('CAPx profile');

  // Define this as a base table – a table that can be described in itself by
  // views (and not just being brought in as a relationship). In reality this
  // is not very useful for this table, as it isn't really a distinct object of
  // its own, but it makes a good example.
  $data['capx_profiles']['table']['base'] = array(
    'field' => 'id', // This is the identifier field for the view.
    'title' => t('CAPx profile'),
    'help' => t('Additional information about a profile.'),
    'weight' => -10,
  );

  // settings table field.
  $data['capx_profiles']['importer'] = array(
    'title' => t('Importer'),
    'help' => t('The machine name of the CAPx importer.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );


  return $data;
}

/**
 * Implements hook_views_data_alter()
 * @param  [type] $data [description]
 * @return [type]       [description]
 */
function stanford_capx_views_data_alter(&$data) {

  // Add a relationship to the capx_profiles table.
  $data['node']['capx_profiles'] = array(
    'title' => t('CAPx profile info relationship'),
    'help' => t('A relationship to the extra profile information from CAP'),
    'relationship' => array(
      'base' => 'capx_profiles', // Table we're joining to.
      'base field' => 'entity_id', // Field on the joined table.
      'field' => 'nid', // Real field name on the 'foo' table.
      'handler' => 'views_handler_relationship',
      'label' => t('Importer'),
      'title' => t('CAPx importer'),
      'help' => t('A relationship between the imported entity and the importer that imported it.'),
    ),
  );

}
