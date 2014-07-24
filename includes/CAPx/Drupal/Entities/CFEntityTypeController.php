<?php
/**
 * @file
 * @author [author] <[email]>
 */
namespace CAPx\Drupal\Entities;

class CFEntityTypeController extends \EntityAPIControllerExportable {

  /**
   * [create description]
   * @param  array  $values [description]
   * @return [type]         [description]
   */
  public function create(array $values = array()) {
    $values += array(
      'label' => '',
      'description' => '',
    );
    return parent::create($values);
  }

  /**
   * Save Task Type.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    parent::save($entity, $transaction);
  }

}
