print('\n'
'**********************************************************************\n'
'enabled:     %d   (1)\n'
'initialized: %d   (0)\n'
'active:      %d   (0)\n'
'**********************************************************************'
%(subs_verif.subs.is_enabled(),
subs_verif.subs.is_initialized(),
subs_verif.subs.is_active()))

subs_verif.subs.initialize()
subs_verif.subs.subscribe()

print('Initialize and Subscribe\n'
'**********************************************************************\n'
'enabled:     %d   (1)\n'
'initialized: %d   (1)\n'
'active:      %d   (1)\n'
'**********************************************************************'
%(subs_verif.subs.is_enabled(),
subs_verif.subs.is_initialized(),
subs_verif.subs.is_active()))

trick.stop(0)
