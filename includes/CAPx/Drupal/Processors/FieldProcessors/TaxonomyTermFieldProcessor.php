<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\FieldProcessors;

class TaxonomyTermFieldProcessor extends FieldTypeProcessor {

  /**
   * Default implementation of put.
   *
   * @see FieldProcessorAbstract::put()
   */
  public function put($data) {
    $data = $this->prepareData($data);

    parent::put($data);
  }

  /**
   * Prepares CAP API data to feet to Drupal field.
   *
   * @param array $data
   *   CAP API field data.
   *
   * @return array
   *   Prepared data.
   */
  public function prepareData($data) {
    $fieldName = $this->getFieldName();
    $fieldInfo = field_info_field($fieldName);
    $vocabulary = taxonomy_vocabulary_machine_name_load($fieldInfo['settings']['allowed_values'][0]['vocabulary']);
    $return = array();

    foreach ($data as $column => $values) {
      foreach ($values as $value) {
        $terms = explode(',', $value);
        if (count($terms) > 1) {
          foreach ($terms as $termName) {
            $return[$column][] = $this->ensureTerm($termName, $vocabulary);
          }
        }
        else {
          $return[$column][] = $this->ensureTerm($value, $vocabulary);
        }
      }
    }

    return $return;
  }

  /**
   * Ensures that term exists.
   *
   * @param string $name
   *   Term name.
   * @param object $vocabulary
   *   Vocabulary to search term in.
   *
   * @return int
   *   Term ID.
   */
  public function ensureTerm($name, $vocabulary) {
    $terms = taxonomy_get_term_by_name($name, $vocabulary->machine_name);
    $term = array_shift($terms);

    if (empty($term->tid)) {
      // Term not found, create and save a new term.
      $term = new \stdClass();
      $term->name = trim($name);
      $term->description = '';
      $term->vid = $vocabulary->vid;
      taxonomy_term_save($term);
    }

    return $term->tid;
  }

}
