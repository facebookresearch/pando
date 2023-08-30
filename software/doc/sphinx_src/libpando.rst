
===============
libpando
===============

libpando has one main public interface:

.. doxygenstruct:: pnd::pando::PandoInterface

There's a factory class that manages a singleton instance of this interface:

.. doxygenclass:: pnd::pando::SingletonPando
   :members:


A typical usage of libpando might look like this:

.. code-block:: cpp

   auto pando = SingletonPando::Get();
   
   // Retrieve the current (default) config
   auto config = pando->GetConfig();
   
   config.device = PandoInterface::DeviceType::HYDRA_HARP_T2;
   config.exp_type =  HYDRA_HARP_T2::ExperimentType::DCS;
   
   // Start experiment 0
   pando->Configure(config);
   auto experiment_handle = pando->Start();
   
   // ... Run for some amount of time
   
   // Double the g2 integration period while experiment is in progress
   config.final_bin_count *= 2;
   pando->Configure(config);
   
   // ... Run for some amount of time
   
   // Stop the experiment.
   experiment_handle.reset();
   
   // Double g2 tau_max (not possible while experiment is in progress)
   config.points_per_level *= 2;
   
   // Start experiment 1
   pando->Configure(config);
   experiment_handle = pando->Start();
   
   // ...

PandoInterface Member Functions
--------------------------------
.. doxygenfunction:: pnd::pando::PandoInterface::Configure

..
   Start() is document explicitly here because PandoInterface inherits it from
   SessionManager<> but does not reimplement it it, so there's no place in the
   codebase where this documentation could go.

.. cpp:function:: pnd::common::SessionHandle pnd::pando::PandoInterface::Start()
   
   Starts an experiment by calling :cpp:func:`Pando::StartImpl`

   Returns a handle whose destructor stops the experiment by calling :cpp:func:`Pando::StopImpl`
   The lifetime of the :cpp:struct:`~pnd::pando::PandoInterface` instance must not end before that of the handle.
   

.. doxygenfunction:: pnd::pando::PandoInterface::GetConfig

.. doxygenfunction:: pnd::pando::PandoInterface::IsHealthy


PandoInterface Member Types
----------------------------

.. doxygenenum:: pnd::pando::PandoInterface::DeviceType

.. doxygenenum:: pnd::pando::PandoInterface::ExperimentType

.. doxygenenum:: pnd::pando::PandoInterface::PF32BitMode

.. doxygenstruct:: pnd::pando::PandoInterface::Config
   :members: 

Miscelaneous
------------

.. doxygentypedef:: pnd::common::SessionHandle

.. doxygenfunction:: pnd::pando::Pando::StartImpl() final

.. doxygenfunction:: pnd::pando::Pando::StopImpl() final

   