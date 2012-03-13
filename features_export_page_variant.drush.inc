<?php

/**
 * @file
 */

/**
 * Implements hook_drush_command().
 */
function features_export_page_variant_drush_command() {
  $items = array();

  $items['features-export-page-variant'] = array(
    'description' => 'Add page manager handlers (variants) to feature.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'arguments' => array(
      'module-name' => 'The feature module to export to.',
      'page-handler-name' => 'A space delimited list of page handler (variant) names.',
    ),
    'examples' => array(
      'features-export-page-variant my-feature my_page_handler_2' => 'Add "my_page_handler_2" to "my-feature".',
    ),
    'aliases' => array('fepv'),
  );

  return $items;
}

/**
 * Implements custom drush command.
 */
function drush_features_export_page_variant($module_name) {
  $export = array();
  $data = array();
  $variant_names = func_get_args();
  array_shift($variant_names);

  module_load_include('inc', 'features', 'features.export');
  ctools_include('features.ctools', 'features');

  // Load existing feature for later usage and bail out if it doesn't exist.
  $feature = feature_load($module_name, TRUE);
  if (empty($feature)) {
    return drush_set_error('PEPV_ERROR', dt('"@feature" is not recognized as a feature.', array('@feature' => $module_name)));
  }

  foreach (array_keys($feature->info['features']) as $component) {
    $export['features'][$component] = drupal_map_assoc($feature->info['features'][$component]);
  }

  $export['dependencies']['ctools'] = 'ctools';
  $export['dependencies']['page_manager'] = 'page_manager';
  $export['features']['ctools']['page_manager:pages_default:1'] = 'page_manager:pages_default:1';

  $count = 0;
  foreach ($variant_names as $variant) {
    if (in_array($variant, $feature->info['features']['page_manager_handlers'])) {
      drush_log(dt('Skipping @variant because it already is part of @feature.', array('@variant' => $variant, '@feature' => $module_name)), 'warning');
    }
    else {
      $export['features']['page_manager_handlers'][$variant] = $variant;
      // Sneaky... We guess where this is actually located... Fragile!
      $data[] = preg_replace('/_\d+$/', '', $variant);
      $count++;
    }
  }

  if ($count < 1) {
    return drush_set_error('PEPV_ERROR', dt('No page variants to add to @feature.', array('@feature' => $module_name)));
  }

  // _features_resolve_dependencies() was added in features-7.x-1.0-beta5.
  // _features_resolve_dependencies() iterates over data and convert to
  // dependencies if already defined elsewhere. It is a nice-to-have but not
  // strictly necessary.
  if (function_exists('_features_resolve_dependencies')) {
    _features_resolve_dependencies($data, $export, $module_name, 'page_manager_handlers');
  }

  $export['dependencies'] = _features_export_minimize_dependencies($export['dependencies'], $module_name);
  $export['dependencies'] = array_merge($export['dependencies'], drupal_map_assoc($feature->info['dependencies']));

  // Sort the entries for consistent ordering in the .info file.
  ksort($export['dependencies']);
  ksort($export['features']);
  ksort($export['features']['ctools']);
  ksort($export['features']['page_manager_handlers']);

  // Export the feature.
  $directory = DRUPAL_ROOT . '/' . drupal_get_path('module', $module_name);
  $files = features_export_render($export, $module_name, TRUE);
  foreach ($files as $extension => $file_contents) {
    if (!in_array($extension, array('module', 'info'))) {
      $extension .= '.inc';
    }
    drush_op('file_put_contents', "{$directory}/{$module_name}.$extension", $file_contents);
  }
  drush_log(dt("Created module: !module in !directory", array('!module' => $module_name, '!directory' => $directory)), 'ok');

  return;
}
