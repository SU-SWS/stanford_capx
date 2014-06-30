<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\FieldProcessors;
use CAPx\Drupal\Processors\FieldProcessors\FieldProcessorAbstract;

/**
 * List of known fields and widgets
 *
 *Type name                Default widget              Widgets
 * datetime                 date_select
 *
 * date                     date_select
 *
 * datestamp                date_select
 *
 * email                    email_textfield
 * field_collection         field_collection_hidden
 *
 * file                     file_generic
 * image                    image_image
 * link_field               link_field
 * list_integer             options_select
 *
 * list_float               options_select
 *
 * list_text                options_select
 *
 * list_boolean             options_buttons
 *
 * number_integer           number
 * number_decimal           number
 * number_float             number
 * taxonomy_term_reference  options_select
 *
 *
 * text                     text_textfield
 * text_long                text_textarea
 * text_with_summary        text_textarea_with_summary
 *
 */


class FieldProcessor extends FieldProcessorAbstract {

  /**
   * [field description]
   * @param  [type] $type [description]
   * @return [type]       [description]
   */
  public function field($type) {

    $entity = $this->getEntity();
    $fieldName = $this->getFieldName();

    // @todo:
    // Allow others to hook in here with their own definitions. Do it by
    // field name.

    switch ($type) {

      case "datetime":
      case "date":
      case "datestamp":
        return new DateFieldProcessor($entity, $fieldName, $type);

      case "email":
        return new EmailFieldProcessor($entity, $fieldName);

      case "field_collection":
        return new FieldCollectionProcessor($entity, $fieldName);

      case "file":
        return new FileFieldProcessor($entity, $fieldName, $type);
      case "image":
        return new ImageFieldProcessor($entity, $fieldName, $type);

      case "link_field":
        return new LinkFieldProcessor($entity, $fieldName);

      case "list_integer":
      case "list_float":
      case "list_text":
      case "list_boolean":
        return new ListFieldProcessor($entity, $fieldName, $type);

      case "number_integer":
      case "number_decimal":
      case "number_float":
        return new NumberFieldProcessor($entity, $fieldName, $type);

      case "taxonomy_term_reference":
        return new TaxonomyTermFieldProcessor($entity, $fieldName);

      case "text":
        return new TextFieldFieldProcessor($entity, $fieldName);

      case "text_long":
      case "text_with_summary":
        return new TextAreaFieldProcessor($entity, $fieldName, $type);

      default:
        return $this;
    }

  }

  /**
   * Widget processing.
   * @param  [type] $type [description]
   * @return [type]              [description]
   */
  public function widget($type) {

    $entity = $this->getEntity();
    $fieldName = $this->getFieldName();

    switch ($type) {

      // Special case for when field has ->value[0]...
      case "text_textarea":
      case 'text_textarea_with_summary':
        return new TextAreaWidgetProcessor($entity, $fieldName);
        break;

      // Downloads and saves a file
      case "image_file":
      case "image_image":
        return new ImageFieldWidgetProcessor($entity, $fieldName);
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
