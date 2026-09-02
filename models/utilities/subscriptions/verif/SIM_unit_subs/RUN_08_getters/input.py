print('\n'
'**********************************************************************\n'
f'enabled:     {subs_verif.subs.is_enabled():d}   (1)\n'
f'initialized: {subs_verif.subs.is_initialized():d}   (0)\n'
f'active:      {subs_verif.subs.is_active():d}   (0)\n'
'**********************************************************************')

subs_verif.subs.initialize()
subs_verif.subs.subscribe()

print('Initialize and Subscribe\n'
'**********************************************************************\n'
f'enabled:     {subs_verif.subs.is_enabled():d}   (1)\n'
f'initialized: {subs_verif.subs.is_initialized():d}   (0)\n'
f'active:      {subs_verif.subs.is_active():d}   (0)\n'
'**********************************************************************')

trick.stop(0)
