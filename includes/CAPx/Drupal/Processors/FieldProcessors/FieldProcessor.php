<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\FieldProcessors;
use CAPx\Drupal\Processors\FieldProcessors\FieldProcessorAbstract;

class FieldProcessor extends FieldProcessorAbstract {

  /**
   * [field description]
   * @param  [type] $type [description]
   * @return [type]       [description]
   */
  public function field($type) {
    $entity = $this->getEntity();
    $fieldName = $this->getFieldName();

    switch ($type) {

      // Special case for when field has ->value[0]...
      case "text_textarea":
      case 'text_textarea_with_summary':
        return new TextAreaFieldProcessor($entity, $fieldName);
        break;

      // Downloads and saves a file
      case "image_file":
      case "image_image":
        return new ImageFieldProcessor($entity, $fieldName);
        break;

      // Default to a generic processor.
      case "text_textfield":
      case "email_textfield":
      default:
        return $this;
        break;
    }

  }

}
