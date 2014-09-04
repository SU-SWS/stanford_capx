<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\FieldProcessors;

class TaxonomyTermFieldProcessor extends FieldTypeProcessor {

  /**
   * Override of default implementation of put.
   * Handles two types of input. A string list of values separated by comma
   * and an array of key => values. This allows for multiple types of selections
   * of data from the CAP API.
   * This function will create new terms if they do not exist.
   * @todo refactor this big ugly function into the two forms of input.
   * @param  array $data An array of CAP API data.
   */
  public function put($data) {

    $entity = $this->getEntity();
    $fieldName = $this->getFieldName();
    $fieldInfo = field_info_field($fieldName);
    $field = $entity->{$fieldName};
    $vocabulary = taxonomy_vocabulary_machine_name_load($fieldInfo['settings']['allowed_values'][0]['vocabulary']);

    // No need for anything fancy when there is nothing to parse :)
    if (count($data) == 1 && empty($data[0])) {
      $field->set(null);
      return;
    }

    // Reformat the jsonpath return data so it works with Durp.
    $data = $this->repackageJsonDataForDrupal($data, $fieldInfo);

    // No valid colums were found. Truncate field.
    if (empty($data)) {
      drupal_set_message('No valid columns found for ' . $fieldName, 'error');
      $field->set(null);
      return;
    }

    // Handle two forms of input.
    // 1. A string list of values separated by a comma
    // 2. An array of key => values where the key is the tag name and value is a boolean

    $save_data = array();

    // 1. Handle a string of comma separated values
    // Re package them into an array so that they match the format for step #2
    foreach ($data as $i => $v) {
      if (is_string($v['tid'])) {
        $opts = explode(",", $v['tid']);
        $count = count($opts);
        $vals = array_fill(0, $count, TRUE);
        $data[$i]['tid'] = array_combine($opts, $vals);
      }
    }

    // 2. Handle arrays of name => bool
    foreach ($data as $index_key => $value) {
      if (is_array($value['tid'])) {

        foreach ($value['tid'] as $termName => $isTrue) {
          if ($isTrue) {

            $termName = trim($termName);
            $terms = taxonomy_get_term_by_name($termName, $vocabulary->machine_name);
            $term = array_pop($terms);

            // If we find a term add the tid to the list
            if (isset($term->tid)) {
              $save_data[] = $term;
            }
            else {
              // Create and save a new term
              $term = new \stdClass();
              $term->name = $termName;
              $term->description = '';
              $term->vid = $vocabulary->vid;
              taxonomy_term_save($term);
              $save_data[] = $term;
            }

          }
        }
      }
    }

    // Only want the first value for one card field
    if ($fieldInfo['cardinality'] == "1") {
      $field->set($save_data[0]->tid);
    }
    else {
      // For everything else give it all.
      $field->set($save_data);
    }

  }


}
