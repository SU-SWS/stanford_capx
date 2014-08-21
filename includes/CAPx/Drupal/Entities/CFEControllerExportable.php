<?php
/**
 * @file
 * @author
 */

namespace CAPx\Drupal\Entities;

class CFEControllerExportable extends \EntityAPIControllerExportable {

  /**
   * [create description]
   * @param  array  $values [description]
   * @return [type]         [description]
   */
  public function create(array $values = array()) {
    global $user;
    $values += array(
      'title' => '',
      'description' => '',
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
      'uid' => $user->uid,
      'module' => 'stanford_capx',
    );
    return parent::create($values);
  }

  /**
   * Save function.
   * @param  [type] $entity      [description]
   * @param  [type] $transaction [description]
   * @return [type]              [description]
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    parent::save($entity, $transaction);
  }

}
