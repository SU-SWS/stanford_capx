<?php
/**
 * @file
 * Stanford CAPx administration pages.
 */

namespace Drupal\stanford_capx\Controller;

use Drupal\Core\Controller\ControllerBase;

/**
 * Controller routines for page example routes.
 */
class AdminPagesController extends ControllerBase {

  /**
   * {@inheritdoc}
   */
  protected function getModuleName() {
    return 'stanford_capx';
  }

  /**
   * Athentication Credentials Input Page.
   *
   * @return [type] [description]
   */
  public function authenticationPage() {
    $content = "";
    return [
      '#markup' => $content,
    ];
  }

  /**
   * Athentication Credentials Input Page.
   *
   * @return [type] [description]
   */
  public function settingsPage() {
    $content = "";
    return [
      '#markup' => $content,
    ];
  }

}
