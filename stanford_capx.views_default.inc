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
  $handler->display->display_options['fields']['settings']['key'] = 'entity_type';
  /* Field: Configuration Entity: Configuration Settings */
  $handler->display->display_options['fields']['settings_1']['id'] = 'settings_1';
  $handler->display->display_options['fields']['settings_1']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['settings_1']['field'] = 'settings';
  $handler->display->display_options['fields']['settings_1']['label'] = 'Bundle Type';
  $handler->display->display_options['fields']['settings_1']['format'] = 'key';
  $handler->display->display_options['fields']['settings_1']['key'] = 'bundle_type';
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
  /* Field: Global: Base url */
  $handler->display->display_options['fields']['base_url']['id'] = 'base_url';
  $handler->display->display_options['fields']['base_url']['table'] = 'views';
  $handler->display->display_options['fields']['base_url']['field'] = 'base_url';
  $handler->display->display_options['fields']['base_url']['label'] = '';
  $handler->display->display_options['fields']['base_url']['exclude'] = TRUE;
  $handler->display->display_options['fields']['base_url']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['base_url']['show_link'] = 0;
  $handler->display->display_options['fields']['base_url']['show_link_options'] = array(
    'link_path' => '',
    'link_text' => '',
    'link_class' => '',
    'link_title' => '',
    'link_rel' => '',
    'link_fragment' => '',
    'link_query' => '',
    'link_target' => '',
  );
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
  $handler->display->display_options['fields']['settings']['alter']['path'] = 'admin/config/capx/mapper/edit/[settings]';
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
  $handler->display->display_options['fields']['meta_1']['key'] = 'lastUpdateHuman';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = 'Operations';
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = '[nothing] | [nothing_1] | <a href="[base_url]/admin/config/capx/importer/sync/[machine_name]?destination=admin/config/capx/importer" title="Synchronize all profiles in this importer from the API">Sync profiles now</a>';
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

  $view = new view();
  $view->name = 'importers_overview_page';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'capx_cfe';
  $view->human_name = 'Importers - Overview page';
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
    'settings' => 'settings',
    'meta_1' => 'meta_1',
    'meta' => 'meta',
    'nothing' => 'nothing',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
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
    'settings' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'meta_1' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'meta' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Field: Global: Base url */
  $handler->display->display_options['fields']['base_url']['id'] = 'base_url';
  $handler->display->display_options['fields']['base_url']['table'] = 'views';
  $handler->display->display_options['fields']['base_url']['field'] = 'base_url';
  $handler->display->display_options['fields']['base_url']['label'] = '';
  $handler->display->display_options['fields']['base_url']['exclude'] = TRUE;
  $handler->display->display_options['fields']['base_url']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['base_url']['show_link'] = 0;
  $handler->display->display_options['fields']['base_url']['show_link_options'] = array(
    'link_path' => '',
    'link_text' => '',
    'link_class' => '',
    'link_title' => '',
    'link_rel' => '',
    'link_fragment' => '',
    'link_query' => '',
    'link_target' => '',
  );
  /* Field: Configuration Entity: Machine name */
  $handler->display->display_options['fields']['machine_name']['id'] = 'machine_name';
  $handler->display->display_options['fields']['machine_name']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['machine_name']['field'] = 'machine_name';
  $handler->display->display_options['fields']['machine_name']['label'] = '';
  $handler->display->display_options['fields']['machine_name']['exclude'] = TRUE;
  $handler->display->display_options['fields']['machine_name']['element_label_colon'] = FALSE;
  /* Field: Configuration Entity: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['title']['alter']['path'] = 'admin/config/capx/importer/edit/[machine_name]';
  /* Field: Configuration Entity: Configuration Settings */
  $handler->display->display_options['fields']['settings']['id'] = 'settings';
  $handler->display->display_options['fields']['settings']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['settings']['field'] = 'settings';
  $handler->display->display_options['fields']['settings']['label'] = 'Mapper';
  $handler->display->display_options['fields']['settings']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['settings']['alter']['path'] = 'admin/config/capx/mapper/edit/[settings]';
  $handler->display->display_options['fields']['settings']['format'] = 'key';
  $handler->display->display_options['fields']['settings']['key'] = 'mapper';
  /* Field: Configuration Entity: Meta Data */
  $handler->display->display_options['fields']['meta_1']['id'] = 'meta_1';
  $handler->display->display_options['fields']['meta_1']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['meta_1']['field'] = 'meta';
  $handler->display->display_options['fields']['meta_1']['label'] = 'Number of profiles';
  $handler->display->display_options['fields']['meta_1']['format'] = 'key';
  $handler->display->display_options['fields']['meta_1']['key'] = 'count';
  /* Field: Configuration Entity: Meta Data */
  $handler->display->display_options['fields']['meta']['id'] = 'meta';
  $handler->display->display_options['fields']['meta']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['meta']['field'] = 'meta';
  $handler->display->display_options['fields']['meta']['label'] = 'Last Update';
  $handler->display->display_options['fields']['meta']['format'] = 'key';
  $handler->display->display_options['fields']['meta']['key'] = 'lastUpdateHuman';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Operations';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = '<a href="[base_url]/admin/config/capx/importer/sync/[machine_name]?destination=admin/config/capx">Update all profiles now</a>';
  /* Sort criterion: Configuration Entity: Title */
  $handler->display->display_options['sorts']['title']['id'] = 'title';
  $handler->display->display_options['sorts']['title']['table'] = 'capx_cfe';
  $handler->display->display_options['sorts']['title']['field'] = 'title';
  /* Filter criterion: Configuration Entity: type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'capx_cfe';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = 'importer';

  /* Display: Importer Overview */
  $handler = $view->new_display('block', 'Importer Overview', 'importers_overview');
  $handler->display->display_options['display_description'] = 'Importer Overview';
  $handler->display->display_options['block_description'] = 'Importer Overview Block';

  $views['importers_overview_page'] = $view;

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  $view = new view();
  $view->name = 'capx_admin_profiles';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'capx_profiles';
  $view->human_name = 'Profiles';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Profiles';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer capx';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'delta' => 'delta',
    'name' => 'title',
    'title_1' => 'title',
    'title' => 'title',
    'entity_type' => 'entity_type',
    'bundle_type' => 'bundle_type',
    'importer_1' => 'importer_1',
    'sync' => 'sync',
    'last_sync' => 'last_sync',
    'edit_link' => 'edit_link',
    'edit_node' => 'edit_node',
    'nothing' => 'nothing',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'delta' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'title_1' => array(
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
    'entity_type' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'bundle_type' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'importer_1' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'sync' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'last_sync' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'edit_link' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'edit_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: CAPx profile: CAPx Profile Node Information */
  $handler->display->display_options['relationships']['node']['id'] = 'node';
  $handler->display->display_options['relationships']['node']['table'] = 'capx_profiles';
  $handler->display->display_options['relationships']['node']['field'] = 'node';
  /* Relationship: CAPx profile: CAPx Profile BEAN Information */
  $handler->display->display_options['relationships']['bean']['id'] = 'bean';
  $handler->display->display_options['relationships']['bean']['table'] = 'capx_profiles';
  $handler->display->display_options['relationships']['bean']['field'] = 'bean';
  /* Relationship: CAPx profile: CAPx Profile User Information */
  $handler->display->display_options['relationships']['user']['id'] = 'user';
  $handler->display->display_options['relationships']['user']['table'] = 'capx_profiles';
  $handler->display->display_options['relationships']['user']['field'] = 'user';
  /* Field: Block: Block Delta */
  $handler->display->display_options['fields']['delta']['id'] = 'delta';
  $handler->display->display_options['fields']['delta']['table'] = 'bean';
  $handler->display->display_options['fields']['delta']['field'] = 'delta';
  $handler->display->display_options['fields']['delta']['relationship'] = 'bean';
  $handler->display->display_options['fields']['delta']['exclude'] = TRUE;
  /* Field: User: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'users';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['relationship'] = 'user';
  $handler->display->display_options['fields']['name']['hide_empty'] = TRUE;
  $handler->display->display_options['fields']['name']['empty_zero'] = TRUE;
  $handler->display->display_options['fields']['name']['overwrite_anonymous'] = TRUE;
  $handler->display->display_options['fields']['name']['anonymous_text'] = ' ';
  /* Field: Block: Title */
  $handler->display->display_options['fields']['title_1']['id'] = 'title_1';
  $handler->display->display_options['fields']['title_1']['table'] = 'bean';
  $handler->display->display_options['fields']['title_1']['field'] = 'title';
  $handler->display->display_options['fields']['title_1']['relationship'] = 'bean';
  $handler->display->display_options['fields']['title_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['title_1']['alter']['path'] = 'block/[delta]/edit';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['relationship'] = 'node';
  /* Field: CAPx profile: Entity Type */
  $handler->display->display_options['fields']['entity_type']['id'] = 'entity_type';
  $handler->display->display_options['fields']['entity_type']['table'] = 'capx_profiles';
  $handler->display->display_options['fields']['entity_type']['field'] = 'entity_type';
  /* Field: CAPx profile: Bundle Type */
  $handler->display->display_options['fields']['bundle_type']['id'] = 'bundle_type';
  $handler->display->display_options['fields']['bundle_type']['table'] = 'capx_profiles';
  $handler->display->display_options['fields']['bundle_type']['field'] = 'bundle_type';
  /* Field: CAPx profile: Importer */
  $handler->display->display_options['fields']['importer_1']['id'] = 'importer_1';
  $handler->display->display_options['fields']['importer_1']['table'] = 'capx_profiles';
  $handler->display->display_options['fields']['importer_1']['field'] = 'importer';
  $handler->display->display_options['fields']['importer_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['importer_1']['alter']['path'] = 'admin/config/capx/importer/edit/[importer_1]';
  /* Field: CAPx profile: Sync Status */
  $handler->display->display_options['fields']['sync']['id'] = 'sync';
  $handler->display->display_options['fields']['sync']['table'] = 'capx_profiles';
  $handler->display->display_options['fields']['sync']['field'] = 'sync';
  $handler->display->display_options['fields']['sync']['not'] = 0;
  /* Field: CAPx profile: Last Sync */
  $handler->display->display_options['fields']['last_sync']['id'] = 'last_sync';
  $handler->display->display_options['fields']['last_sync']['table'] = 'capx_profiles';
  $handler->display->display_options['fields']['last_sync']['field'] = 'last_sync';
  $handler->display->display_options['fields']['last_sync']['date_format'] = 'time ago';
  $handler->display->display_options['fields']['last_sync']['second_date_format'] = 'long';
  /* Field: Block: Edit link */
  $handler->display->display_options['fields']['edit_link']['id'] = 'edit_link';
  $handler->display->display_options['fields']['edit_link']['table'] = 'bean';
  $handler->display->display_options['fields']['edit_link']['field'] = 'edit_link';
  $handler->display->display_options['fields']['edit_link']['relationship'] = 'bean';
  $handler->display->display_options['fields']['edit_link']['exclude'] = TRUE;
  $handler->display->display_options['fields']['edit_link']['text'] = 'EDIT';
  /* Field: Content: Edit link */
  $handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['relationship'] = 'node';
  $handler->display->display_options['fields']['edit_node']['exclude'] = TRUE;
  $handler->display->display_options['fields']['edit_node']['text'] = 'EDIT';
  /* Field: User: Edit link */
  $handler->display->display_options['fields']['edit_node_1']['id'] = 'edit_node_1';
  $handler->display->display_options['fields']['edit_node_1']['table'] = 'users';
  $handler->display->display_options['fields']['edit_node_1']['field'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node_1']['relationship'] = 'user';
  $handler->display->display_options['fields']['edit_node_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['edit_node_1']['text'] = 'EDIT';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Operations';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = '[edit_node][edit_link][edit_node_1] | View on CAP';
  /* Filter criterion: CAPx profile: Bundle Type */
  $handler->display->display_options['filters']['bundle_type']['id'] = 'bundle_type';
  $handler->display->display_options['filters']['bundle_type']['table'] = 'capx_profiles';
  $handler->display->display_options['filters']['bundle_type']['field'] = 'bundle_type';
  $handler->display->display_options['filters']['bundle_type']['exposed'] = TRUE;
  $handler->display->display_options['filters']['bundle_type']['expose']['operator_id'] = 'bundle_type_op';
  $handler->display->display_options['filters']['bundle_type']['expose']['label'] = 'Bundle Type';
  $handler->display->display_options['filters']['bundle_type']['expose']['operator'] = 'bundle_type_op';
  $handler->display->display_options['filters']['bundle_type']['expose']['identifier'] = 'bundle_type';
  $handler->display->display_options['filters']['bundle_type']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: CAPx profile: Entity Type */
  $handler->display->display_options['filters']['entity_type']['id'] = 'entity_type';
  $handler->display->display_options['filters']['entity_type']['table'] = 'capx_profiles';
  $handler->display->display_options['filters']['entity_type']['field'] = 'entity_type';
  $handler->display->display_options['filters']['entity_type']['exposed'] = TRUE;
  $handler->display->display_options['filters']['entity_type']['expose']['operator_id'] = 'entity_type_op';
  $handler->display->display_options['filters']['entity_type']['expose']['label'] = 'Entity Type';
  $handler->display->display_options['filters']['entity_type']['expose']['operator'] = 'entity_type_op';
  $handler->display->display_options['filters']['entity_type']['expose']['identifier'] = 'entity_type';
  $handler->display->display_options['filters']['entity_type']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: CAPx profile: Importer */
  $handler->display->display_options['filters']['importer']['id'] = 'importer';
  $handler->display->display_options['filters']['importer']['table'] = 'capx_profiles';
  $handler->display->display_options['filters']['importer']['field'] = 'importer';
  $handler->display->display_options['filters']['importer']['exposed'] = TRUE;
  $handler->display->display_options['filters']['importer']['expose']['operator_id'] = 'importer_op';
  $handler->display->display_options['filters']['importer']['expose']['label'] = 'Importer';
  $handler->display->display_options['filters']['importer']['expose']['operator'] = 'importer_op';
  $handler->display->display_options['filters']['importer']['expose']['identifier'] = 'importer';
  $handler->display->display_options['filters']['importer']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Profiles */
  $handler = $view->new_display('page', 'Profiles', 'profiles');
  $handler->display->display_options['path'] = 'admin/config/capx/profiles';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Profiles';
  $handler->display->display_options['menu']['description'] = 'CAPx Profiles';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;


  $views['capx_admin_profiles'] = $view;

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  return $views;
}
