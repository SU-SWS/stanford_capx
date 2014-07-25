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
  /* Field: Configuration Entity: Machine-readable name */
  $handler->display->display_options['fields']['machine_name']['id'] = 'machine_name';
  $handler->display->display_options['fields']['machine_name']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['machine_name']['field'] = 'machine_name';
  $handler->display->display_options['fields']['machine_name']['exclude'] = TRUE;
  /* Field: Configuration Entity: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'capx_cfe';
  $handler->display->display_options['fields']['title']['field'] = 'title';
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
  /* Filter criterion: Configuration Entity: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'capx_cfe';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'mapper' => 'mapper',
  );

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block');
  $handler->display->display_options['block_description'] = 'Manage Mappers Table';

  // Add view to return array.
  $views['capx_mappers'] = $view;

  return $views;
}
