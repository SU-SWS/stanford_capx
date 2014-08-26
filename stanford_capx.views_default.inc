<?php
/**
 * @file
 * @author [author] <[email]>
 */

function stanford_capx_views_default_views() {
  $views = array();

  $view = new view();
  $view->name = 'capx_mappers';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'capx_cfe';
  $view->human_name = 'Mappers';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Manage Mappers';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'created' => 'created',
    'machine_name' => 'machine_name',
    'title' => 'title',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'created' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'machine_name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'title' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['label'] = 'Sorry';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'There are no available mappers. Click the create mapper link above to get started.';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  /* Field: Configuration Entity: Machine name */
  $handler->display->display_options['fields']['machine_name']['id'] = 'machine_name';
  $handler->display->display_options['fields']['machine_name']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['machine_name']['field'] = 'machine_name';
  $handler->display->display_options['fields']['machine_name']['exclude'] = TRUE;
  /* Field: Configuration Entity: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  /* Field: Configuration Entity: Configuration Settings */
  $handler->display->display_options['fields']['settings']['id'] = 'settings';
  $handler->display->display_options['fields']['settings']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['settings']['field'] = 'settings';
  $handler->display->display_options['fields']['settings']['label'] = 'Entity Type';
  $handler->display->display_options['fields']['settings']['format'] = 'key';
  $handler->display->display_options['fields']['settings']['key'] = 'entityType';
  /* Field: Configuration Entity: Configuration Settings */
  $handler->display->display_options['fields']['settings_1']['id'] = 'settings_1';
  $handler->display->display_options['fields']['settings_1']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['settings_1']['field'] = 'settings';
  $handler->display->display_options['fields']['settings_1']['label'] = 'Bundle Type';
  $handler->display->display_options['fields']['settings_1']['format'] = 'key';
  $handler->display->display_options['fields']['settings_1']['key'] = 'bundleType';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Edit';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'Edit';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/config/capx/mapper/edit/[machine_name]';
  $handler->display->display_options['fields']['nothing']['alter']['absolute'] = TRUE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = 'Delete';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'Delete';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'admin/config/capx/mapper/delete/[machine_name]';
  $handler->display->display_options['fields']['nothing_1']['alter']['absolute'] = TRUE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = 'Operations';
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = '[nothing] | [nothing_1]';
  /* Sort criterion: Configuration Entity: Title */
  $handler->display->display_options['sorts']['title']['id'] = 'title';
  $handler->display->display_options['sorts']['title']['table'] = 'capx_cfe';
  $handler->display->display_options['sorts']['title']['field'] = 'title';
  /* Sort criterion: Configuration Entity: Created */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'capx_cfe';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  /* Filter criterion: Configuration Entity: type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'capx_cfe';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = 'mapper';

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'mappers_manage_block');
  $handler->display->display_options['block_description'] = 'Manage Mappers Table';

  // Add view to return array.
  $views['capx_mappers'] = $view;

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  // Importers
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  $view = new view();
  $view->name = 'capx_importers';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'capx_cfe';
  $view->human_name = 'Importers';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Importers';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'machine_name' => 'machine_name',
    'title' => 'title',
    'nothing' => 'nothing',
    'nothing_1' => 'nothing_1',
    'nothing_2' => 'nothing_2',
  );
  $handler->display->display_options['style_options']['default'] = 'title';
  $handler->display->display_options['style_options']['info'] = array(
    'machine_name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_1' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_2' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['label'] = 'Sorry';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'Sorry, there are no importers available. To get started click on the create new importer link above.';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  /* Field: Configuration Entity: Machine name */
  $handler->display->display_options['fields']['machine_name']['id'] = 'machine_name';
  $handler->display->display_options['fields']['machine_name']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['machine_name']['field'] = 'machine_name';
  $handler->display->display_options['fields']['machine_name']['exclude'] = TRUE;
  /* Field: Configuration Entity: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['title']['alter']['path'] = 'admin/config/capx/importer/edit/[machine_name]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Edit';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'Edit';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/config/capx/importer/edit/[machine_name]';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = 'Delete';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'Delete';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'admin/config/capx/importer/delete/[machine_name]';
  /* Field: Configuration Entity: Configuration Settings */
  $handler->display->display_options['fields']['settings']['id'] = 'settings';
  $handler->display->display_options['fields']['settings']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['settings']['field'] = 'settings';
  $handler->display->display_options['fields']['settings']['label'] = 'Mapper Name';
  $handler->display->display_options['fields']['settings']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['settings']['alter']['path'] = 'admin/config/mapper/edit/[settings]';
  $handler->display->display_options['fields']['settings']['format'] = 'key';
  $handler->display->display_options['fields']['settings']['key'] = 'mapper';
  /* Field: Configuration Entity: Meta Data */
  $handler->display->display_options['fields']['meta']['id'] = 'meta';
  $handler->display->display_options['fields']['meta']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['meta']['field'] = 'meta';
  $handler->display->display_options['fields']['meta']['label'] = 'Number of profiles';
  $handler->display->display_options['fields']['meta']['format'] = 'key';
  $handler->display->display_options['fields']['meta']['key'] = 'count';
  /* Field: Configuration Entity: Meta Data */
  $handler->display->display_options['fields']['meta_1']['id'] = 'meta_1';
  $handler->display->display_options['fields']['meta_1']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['meta_1']['field'] = 'meta';
  $handler->display->display_options['fields']['meta_1']['label'] = 'Last Update';
  $handler->display->display_options['fields']['meta_1']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['meta_1']['format'] = 'key';
  $handler->display->display_options['fields']['meta_1']['key'] = 'lastUpdate';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = 'Operations';
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = '[nothing] | [nothing_1] | Sync';
  /* Filter criterion: Configuration Entity: type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'capx_cfe';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = 'importer';

  /* Display: Importer Manage Block */
  $handler = $view->new_display('block', 'Importer Manage Block', 'importer_manage_block');

  $views['capx_importers'] = $view;

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  return $views;
}
