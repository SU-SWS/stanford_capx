<?php
/**
 * @file
 * API Documentation
 */

/**
 * [hook_capx_pre_map_field description]
 * @see  EntityMapper.php
 * @param  [type] $entity            [description]
 * @param  [type] $field_name        [description]
 * @param  [type] $remote_data_paths [description]
 * @return [type]                    [description]
 */
function hook_capx_pre_map_field_alter(&$entity, &$field_name, &$remote_data_paths) {

}

/**
 * [hook_capx_post_map_field_alter description]
 * @see  EntityMapper.php
 * @param  [type] $entity     [description]
 * @param  [type] $field_name [description]
 * @return [type]             [description]
 */
function hook_capx_post_map_field_alter(&$entity, &$field_name) {

}

/**
 * [hook_capx_new_fc_alter description]
 * @see  FieldCollectionProcessor.php
 * @param  [type] $field_collection [description]
 * @return [type]                   [description]
 */
function hook_capx_new_fc_alter(&$field_collection) {

}

/**
 * [hook_capx_pre_update_entity description]
 * @see  EntityProcessor.php
 * @param  [type] $entity [description]
 * @param  [type] $data   [description]
 * @param  [type] $mapper [description]
 * @return [type]         [description]
 */
function hook_capx_pre_update_entity_alter(&$entity, &$data, &$mapper) {

}

/**
 * [hook_capx_post_update_entity description]
 * @see  EntityProcessor.php
 * @param  [type] $entity [description]
 * @return [type]         [description]
 */
function hook_capx_post_update_entity_alter(&$entity) {

}

/**
 * [hook_capx_pre_entity_create description]
 * @see  EntityProcessor.php
 * @param  [type] $properties  [description]
 * @param  [type] $entity_type [description]
 * @param  [type] $bundle_type [description]
 * @param  [type] $mapper      [description]
 * @return [type]              [description]
 */
function hook_capx_pre_entity_create_alter(&$properties, &$entity_type, &$bundle_type, &$mapper) {

}

/**
 * [hook_capx_post_entity_create_alter description]
 * @see  EntityProcessor.php
 * @param  [type] $entity [description]
 * @return [type]         [description]
 */
function hook_capx_post_entity_create_alter(&$entity) {

}

/**
 * [hook_capx_pre_property_set description]
 * @see  PropertyProcessorAbstract.php
 * @param  [type] $entity        [description]
 * @param  [type] $data          [description]
 * @param  [type] $property_name [description]
 * @return [type]                [description]
 */
function hook_capx_pre_property_set_alter(&$entity, &$data, &$property_name) {

}

/**
 * [hook_capx_field_processor_pre_set_alter description]
 * @param  [type] $entity     [description]
 * @param  [type] $data       [description]
 * @param  [type] $field_name [description]
 * @return [type]             [description]
 */
function hook_capx_field_processor_pre_set_alter(&$entity, &$data, &$field_name) {

}

/**
 * [hook_capx_pre_fetch_remote_file_alter description]
 * @see  FileFieldProcessor.php
 * @param  [type] $data [description]
 * @return [type]       [description]
 */
function hook_capx_pre_fetch_remote_file_alter(&$data) {

}

/**
 * [hook_capx_post_save_remote_file_alter description]
 * @see  FileFieldProcessor.php
 * @param  [type] $file     [description]
 * @param  [type] $filename [description]
 * @return [type]           [description]
 */
function hook_capx_post_save_remote_file_alter(&$file, $filename) {

}

/**
 * [hook_capx_field_type_processor_widget_alter description]
 * @see  FieldTypeProcessor.php
 * @param  [type] $widget [description]
 * @param  [type] $type   [description]
 * @return [type]         [description]
 */
function hook_capx_field_type_processor_widget_alter(&$widget, $type) {

}

/**
 * [hook_capx_field_processor_field_alter description]
 * @see  FieldProcessor.php
 * @param  [type] $processor  [description]
 * @param  [type] $type       [description]
 * @param  [type] $field_name [description]
 * @param  [type] $entity     [description]
 * @return [type]             [description]
 */
function hook_capx_field_processor_field_alter(&$processor, $type, $field_name, &$entity) {

}

/**
 * [hook_capx_field_processor_widget_alter description]
 * @see  FieldProcessor.php
 * @param  [type] $processor  [description]
 * @param  [type] $type       [description]
 * @param  [type] $field_name [description]
 * @param  [type] $entity     [description]
 * @return [type]             [description]
 */
function hook_capx_field_processor_widget_alter(&$processor, $type, $field_name, &$entity) {

}

/**
 * Act on the results from a API server request. This is the first chance you
 * get prior to any processing on the item.
 * @param  [array] $results  an array of results from a request from the server.
 * @param  [EntityImporter] $importer the entity importer that made the request.
 * @return [type]           [description]
 */
function hook_capx_preprocess_results_alter(&$results, $importer) {

}


































