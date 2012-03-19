# Drush features export page variant #

This is a Drush plugin for experienced Drupal developers.

It is a workaround for putting panel page variants in to another
feature than the "original" panel page.

This is intended as a temporary solution for getting the work done
while a better solution is incorporated to features and/or ctools.

Be sure to read the "Caveats" section below. And remember to have
backups and/or version control in place.

The plugin has been tested to work with both Drush 4.5 and Drush 5.x.

## Use case ##

You have a panel page that is either defined in a feature (with the
[features](http://drupal.org/project/features) module) and you want to
add a page variant of that page - except you would like to store it in
some other feature than where the original page was defined.

Currently features is unable to save your variant to another feature -
although it is very well capable of keeping it up to update once you
manage to have it saved elsewhere.

This Drush plugin saves your panel page variant to another feature and
lets you get on with your work.

## Usage ##

Go ahead and create your page variant as usual.

Then add "my\_page\_handler_2" to "my-feature" with the help of
[Drush](http://drupal.org/project/drush):
  
`$ drush features-export-page-variant my-feature my_page_handler_2`

or using the much shorter command alias, fepv:

`$ drush fepv my-feature my_page_handler_2`

If you would like to copy an existing page variant you can do:

`$ drush fepv my-feature --new-page=my_page_handler_2 my_page_handler_`

This will make a copy of the existing page variant "my\_page\_handler"
and store a copy named "my\_page\_handler\2" in the feature module
"my-feature".

## Finding the page handler name ##

You can locate the page handler name from the export tab of the
variant - look for `$handler->name`:

![Find page_handler name from URL](https://github.com/DBCDK/drush-features-export-page-variant/raw/master/demo2.png)

## Caveats ##

*   The plugin will try to add a dependency on the feature that the
    "original" page is located in. But it does this by guessing!
  
    If the variant you want to export is named `my_page_handler_2` it
    will make a guess and decide that the original must be
    `my_page_handler` and add a dependency on the feature where that
    is located.
  
*   If you are more than one developer adding variants at the same
    time you might end up with more than one instance of a handler
    named `my_page_handler_2`.
  
    Be careful. Talk to each other and make sure your code is
    up-to-date and do a thorough review of the code before pushing to
    a common repository.
  
    If your end up with several page handlers with the same name you
    can edit the feature and renamed the page handler name manually.
    The name should occur one time in `my-feature.info` and two times
    in `my-feature.pages_default.inc`.

*   Trying to add a non-existing page variant will not produce any
    error messages or warnings.
  
    Even worse it will leave your feature in an inconsistent state.
    `drush feature-update my-feature` will probably bring it in to a
    consistent state again.

*   Adding a page variant that is already handled by another feature
    will not produce any error messages or warnings.
  
    Even worse it will leave your features/site in an inconsistent
    state.

*   You cannot remove a page variant from a feature again. At least
    not from the administrative interface or from Drush.
  
    You will have to edit the feature module (as in change the source
    code) yourself to remove the page variant.
    
*   The feature you want to add the variant to must exist and must be
    recognizable as a feature. This means the feature module must have
    a `my_feature.features.inc` file. Creating a new feature from the
    UI and _not_ adding anything to the feature will _not_ add a
    `my_feature.features.inc` and will hence _not_ be recognized.

*   What we do is actually also equivalent of a `drush feature-update`
    so other changes to your feature will be updated as well.

*   Page variants of pages with `$handler->task = 'page'` (typically
    most of your pages) behaves a bit different. Please see the
    section below.
	
## Caveats with page tasks ##

Page variants of pages with `$handler->task = 'page'` (typically most
of your pages) behaves a bit different than other pages.

The first challenge is that although you have managed to export the
page into another feature the feature containing the original page
will still be marked as overridden.

A work around is to apply
[this patch](https://raw.github.com/gist/2044786/6b4ef8173f080715a8752067f7b1511a99b8b816/ctools-page_manager_load_task_handlers_alter.patch)
to [ctools](http://drupal.org/project/ctools) and adding this
hook_alter() to your feature module:

    /**
     * Implements hook_page_manager_load_task_handlers_alter().
     *
     * When page manager loads task handlers for pages we remove the page handlers
     * that are handled by features in this module.
     *
     * Requires a patch to ctools/page_manager.module (call drupal_alter).
     */
    function my_feature_page_manager_load_task_handlers_alter(&$handlers, $task, $subtask_id = NULL) {
      // If we are on an admin/structure/features* page or we are being called
      // through drush features we will filter out page handlers.
      $in_drush_features = FALSE;
    
      if (function_exists('drush_get_command')) {
        $command = drush_get_command();
        if (isset($command['drupal dependencies']) && in_array('features', $command['drupal dependencies'])) {
          $in_drush_features = TRUE;
        }
      };
    
      if (preg_match('/admin\/structure\/features/', $_GET['q']) || $in_drush_features) {
        // Load the page handlers handled by this feature module.
        $my_feature_handlers = array_keys(my_feature_default_page_manager_handlers());
    
        if ($task['name'] == 'page') {
          foreach ($handlers as $handler) {
            if (in_array($handler->name, $my_feature_handlers)) {
              unset($handlers[$handler->name]);
            }
          }
        }
      }
    }


The other problem is that the page handler itself will not always be
exported if it is a copy of an existing variant. In these cases you
will have to copy'n'paste the handler from the export tab in the UI
and paste it into the feature module yourself.

## The "real" solution ##

This is some of the issues related to integrating a similar solution
in features/ctools/panels:

* [Ability to set variant machine name in Panels UI](http://drupal.org/node/813754)
* [Exported panel variants as features are incompatible with each other](http://drupal.org/node/740074)

