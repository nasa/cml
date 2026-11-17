from veras.element import *
from veras.utils.ListUtil import ListUtil
from veras.utils.PlotUtil import PlotUtil


class Analysis_vehicle_prescribed_attitude(Analysis):
    """
    :ref:`Analysis_vehicle_prescribed_attitude`

    RPOD Dispersion Analysis object - tabulates initial dispersion and dispersions before and after burns.
    """

    #########################################
    # METHOD (BUILT-IN): __init__
    #########################################

    def __init__(self, *args, **kwargs):
        # Initialize property values for this object
        self._PlotStateID = False

        # Update parent object (Analysis) properties
        self._Name = 'Analysis_vehicle_prescribed_attitude'
        self._Description = 'RPOD Lidar visualizations'
        self._Groups = []

        # Pass the inputs to parent "Analysis" class
        # This will set the key-value argument pairs from constructor
        super().__init__(*args, **kwargs)

    #########################################
    # PROPERTY: plot_angles_rates
    #########################################
    @property
    def plot_angles_rates(self):
        """
        Flag to plot State ID
        """
        return self._plot_angles_rates

    @plot_angles_rates.setter
    def plot_angles_rates(self, value):
        # Perform any validation then assignment
        ClassUtil.validateAttribute(value, dataType=bool)
        self._plot_angles_rates = value

    #########################################
    # METHOD: Define Children
    #########################################
    def _defineChildren(self):
        """
        Description of object's children
        """

    #########################################
    # METHOD: Define Data Variables
    #########################################
    def _defineDataVariables(self):
        """
        Description of object's data variables
        """
        self.addDataVariable(
            'test.att_manager.perturbation.prescribedAng', Alias='perturbAng', LoadTRK=True
        )
        self.addDataVariable(
            'test.att_manager.perturbation.deltaLatVelDP', Alias='deltaLatVelDP', LoadTRK=True
        )
        self.addDataVariable(
            'test.att_manager.using_perturbation', Alias='using_perturbation', LoadTRK=True
        )
        self.addDataVariable(
            'test.att_manager.using_maneuver', Alias='using_maneuver', LoadTRK=True
        )
        self.addDataVariable(
            'test.att_manager.quatRefToBody.scalar', Alias='quatRefToBody_scalar', LoadTRK=True
        )
        self.addDataVariable(
            'test.att_manager.quatRefToBody.vector', Alias='quatRefToBody_vector', LoadTRK=True
        )
        self.addDataVariable(
            'test.lvlh_frame.frame.state.rot.Q_parent_this.scalar', Alias='quatInertialToRef_scalar', LoadTRK=True
        )
        self.addDataVariable(
            'test.lvlh_frame.frame.state.rot.Q_parent_this.vector', Alias='quatInertialToRef_vector', LoadTRK=True
        )
        self.addDataVariable(
            'test.body.composite_body.state.rot.Q_parent_this.scalar',
            Alias='Q_parent_this_scalar',
            LoadTRK=True,
        )
        self.addDataVariable(
            'test.body.composite_body.state.rot.Q_parent_this.vector',
            Alias='Q_parent_this_vector',
            LoadTRK=True,
        )
        self.addDataVariable(
            'test.body.composite_body.state.rot.ang_vel_this',
            Alias='body_ang_vel_this',
            LoadTRK=True,
        )
        self.addDataVariable(
            'test.att_manager.w_BodyToRef_Body',
            Alias='w_BodyToRef_Body',
            LoadTRK=True,
        )

    #########################################
    # METHOD: Define Computations
    #########################################
    def _defineComputations(self):
        """
        Description of object's internal computations
        """
        self.name = self.querySimulationData('Name')
        self.Q_parent_this = TimeSeriesQuaternion(
            scalar=self.TSData.Q_parent_this_scalar,
            vector=-self.TSData.Q_parent_this_vector,
        )
        self.comp_body_ang = self.Q_parent_this.quat2angle(sequence='xyz')

    #########################################
    # METHOD: Define Visualization
    #########################################
    def _defineVisualization(self):
        """
        Creates plots
        """

        def plot_mnvr_attitude(
            collectionIndex=[], title="", xlim=[0, 1000]
        ):
            fig, ax = plt.subplots(4, 1)
            for cIdx in collectionIndex:
                time, quatRefToBody_scalar = self.TSData.quatRefToBody_scalar[cIdx]
                time, quatRefToBody_vector = self.TSData.quatRefToBody_vector[cIdx]
                ax[0].plot(
                    time.squeeze(),
                    quatRefToBody_scalar.squeeze(),
                    'r',
                    label='q_s',
                )
                ax[1].plot(
                    time.squeeze(),
                    quatRefToBody_vector[:, 0].squeeze(),
                    'g',
                    label='q_v1',
                )
                ax[2].plot(
                    time.squeeze(),
                    quatRefToBody_vector[:, 1].squeeze(),
                    'b',
                    label='q_v2',
                )
                ax[3].plot(
                    time.squeeze(),
                    quatRefToBody_vector[:, 2].squeeze(),
                    'b',
                    label='q_v3',
                )
            ax[0].set_ylabel("q_scalar")
            ax[1].set_ylabel("q_vector_1")
            ax[2].set_ylabel("q_vector_2")
            ax[3].set_ylabel("q_vector_3")

            for idx in range(4):
                ax[idx].set_xlabel("Time (s)")
                ax[idx].set_xlim(xlim)
                ax[idx].set_xticks(range(xlim[0], xlim[1] + 1, 100))
                ax[idx].legend(loc='lower right', fontsize=20)
                ax[idx].grid(visible=True, which='both', color='k', alpha=1.0)
                for item in (
                    [ax[idx].title, ax[idx].xaxis.label, ax[idx].yaxis.label]
                    + ax[idx].get_xticklabels()
                    + ax[idx].get_yticklabels()
                ):
                    item.set_fontsize(30)
            fig.set_figheight(15)
            fig.set_figwidth(20)
            self.addVisualization(
                ReportFigure(fig, Caption=f'{title}: Attitude from Ref to Nominal Body')
            )

        def plot_inertial_to_ref_attitude(
            collectionIndex=[], title="", xlim=[0, 1000]
        ):
            fig, ax = plt.subplots(4, 1)
            for cIdx in collectionIndex:
                time, quatInertialToRef_scalar = self.TSData.quatInertialToRef_scalar[cIdx]
                time, quatInertialToRef_vector = self.TSData.quatInertialToRef_vector[cIdx]
                ax[0].plot(
                    time.squeeze(),
                    quatInertialToRef_scalar.squeeze(),
                    'r',
                    label='q_s',
                )
                ax[1].plot(
                    time.squeeze(),
                    quatInertialToRef_vector[:, 0].squeeze(),
                    'g',
                    label='q_v1',
                )
                ax[2].plot(
                    time.squeeze(),
                    quatInertialToRef_vector[:, 1].squeeze(),
                    'b',
                    label='q_v2',
                )
                ax[3].plot(
                    time.squeeze(),
                    quatInertialToRef_vector[:, 2].squeeze(),
                    'b',
                    label='q_v3',
                )
            ax[0].set_ylabel("q_scalar")
            ax[1].set_ylabel("q_vector_1")
            ax[2].set_ylabel("q_vector_2")
            ax[3].set_ylabel("q_vector_3")

            for idx in range(4):
                ax[idx].set_xlabel("Time (s)")
                ax[idx].set_xlim(xlim)
                ax[idx].set_xticks(range(xlim[0], xlim[1] + 1, 100))
                ax[idx].legend(loc='lower right', fontsize=20)
                ax[idx].grid(visible=True, which='both', color='k', alpha=1.0)
                for item in (
                    [ax[idx].title, ax[idx].xaxis.label, ax[idx].yaxis.label]
                    + ax[idx].get_xticklabels()
                    + ax[idx].get_yticklabels()
                ):
                    item.set_fontsize(30)
            fig.set_figheight(15)
            fig.set_figwidth(20)
            self.addVisualization(
                ReportFigure(fig, Caption=f'{title}: Attitude from Inertial to custom Ref Frame R')
            )

        def plot_body_attitude(
            collectionIndex=[], title="", xlim=[0, 1000]
        ):
            fig, ax = plt.subplots(4, 1)
            for cIdx in collectionIndex:
                time, Q_parent_this_scalar = self.TSData.Q_parent_this_scalar[cIdx]
                time, Q_parent_this_vector = self.TSData.Q_parent_this_vector[cIdx]
                ax[0].plot(
                    time.squeeze(),
                    Q_parent_this_scalar.squeeze(),
                    'r',
                    label='q_s',
                )
                ax[1].plot(
                    time.squeeze(),
                    Q_parent_this_vector[:, 0].squeeze(),
                    'g',
                    label='q_v1',
                )
                ax[2].plot(
                    time.squeeze(),
                    Q_parent_this_vector[:, 1].squeeze(),
                    'b',
                    label='q_v2',
                )
                ax[3].plot(
                    time.squeeze(),
                    Q_parent_this_vector[:, 2].squeeze(),
                    'b',
                    label='q_v3',
                )
            ax[0].set_ylabel("q_scalar")
            ax[1].set_ylabel("q_vector_1")
            ax[2].set_ylabel("q_vector_2")
            ax[3].set_ylabel("q_vector_3")

            for idx in range(4):
                ax[idx].set_xlabel("Time (s)")
                ax[idx].set_xlim(xlim)
                ax[idx].set_xticks(range(xlim[0], xlim[1] + 1, 100))
                ax[idx].legend(loc='lower right', fontsize=20)
                ax[idx].grid(visible=True, which='both', color='k', alpha=1.0)
                for item in (
                    [ax[idx].title, ax[idx].xaxis.label, ax[idx].yaxis.label]
                    + ax[idx].get_xticklabels()
                    + ax[idx].get_yticklabels()
                ):
                    item.set_fontsize(30)
            fig.set_figheight(15)
            fig.set_figwidth(20)
            self.addVisualization(
                ReportFigure(fig, Caption=f'{title}: Attitude from Inertial to Perturbed Body')
            )

        def plot_perturbation_angles_rates(
            collectionIndex=[], title="", xlim=[0, 1000]
        ):
            fig, ax = plt.subplots(2, 1)
            for cIdx in collectionIndex:
                time, data = self.TSData.perturbAng[cIdx]
                ax[0].plot(
                    time.squeeze(),
                    data[:, 0].convertUnits('degrees').squeeze(),
                    'r',
                    label='Roll',
                )
                ax[0].plot(
                    time.squeeze(),
                    data[:, 1].convertUnits('degrees').squeeze(),
                    'g',
                    label='Pitch',
                )
                ax[0].plot(
                    time.squeeze(),
                    data[:, 2].convertUnits('degrees').squeeze(),
                    'b',
                    label='Yaw',
                )
                time, data = self.TSData.body_ang_vel_this[cIdx]
                ax[1].plot(
                    time.squeeze(),
                    data[:, 0].convertUnits('degrees/s').squeeze(),
                    'r',
                    label='X',
                )
                ax[1].plot(
                    time.squeeze(),
                    data[:, 1].convertUnits('degrees/s').squeeze(),
                    'g',
                    label='Y',
                )
                ax[1].plot(
                    time.squeeze(),
                    data[:, 2].convertUnits('degrees/s').squeeze(),
                    'b',
                    label='Z',
                )
            ax[0].set_ylabel("Body Attitude RPY (deg)")
            ax[1].set_ylabel("Body Rates (deg/s)")
            for idx in range(2):
                ax[idx].set_xlabel("Time (s)")
                ax[idx].set_xlim(xlim)
                ax[idx].set_xticks(range(xlim[0], xlim[1] + 1, 100))
                ax[idx].legend(loc='lower right', fontsize=20)
                ax[idx].grid(visible=True, which='both', color='k', alpha=1.0)
                for item in (
                    [ax[idx].title, ax[idx].xaxis.label, ax[idx].yaxis.label]
                    + ax[idx].get_xticklabels()
                    + ax[idx].get_yticklabels()
                ):
                    item.set_fontsize(30)
            fig.set_figheight(15)
            fig.set_figwidth(20)
            self.addVisualization(
                ReportFigure(fig, Caption=f'{title}: Body Attitude and Rates')
            )

        def plot_phase_portrait(
            collectionIndex=[], title="", xlim=[5.0, 5.0, 5.0], ylim=[0.2, 0.2, 0.2]
        ):
            fig, ax = plt.subplots(3, 1)
            ylim_scaled = np.array(ylim)
            ylim_scaled *= 1.1
            xlim_scaled = np.array(xlim)
            xlim_scaled *= 1.0
            for cIdx in collectionIndex:
                time, data_ang = self.comp_body_ang[cIdx]
                time, data_vel = self.TSData.w_BodyToRef_Body[cIdx]
                ax[0].plot(
                    data_ang[:, 0].convertUnits('degrees').squeeze(),
                    data_vel[:, 0].convertUnits('degrees/s').squeeze(),
                    'r',
                    label='Roll',
                )
                ax[1].plot(
                    data_ang[:, 1].convertUnits('degrees').squeeze(),
                    data_vel[:, 1].convertUnits('degrees/s').squeeze(),
                    'g',
                    label='Pitch',
                )
                ax[2].plot(
                    data_ang[:, 2].convertUnits('degrees').squeeze(),
                    data_vel[:, 2].convertUnits('degrees/s').squeeze(),
                    'b',
                    label='Yaw',
                )
            label = ["Roll", "Pitch", "Yaw"]  
            for idx in range(3):
                ax[idx].set_xlabel(label[idx] + " (deg)")
                ax[idx].set_xlim([-xlim_scaled[idx], xlim_scaled[idx]])
                ax[idx].set_xticks(
                     [-xlim[idx], -xlim[idx] / 2, 0.0, xlim[idx] / 2, xlim[idx]]
                )
                ax[idx].set_ylabel(label[idx] + " Rate (deg/s)")
                ax[idx].set_ylim([-ylim_scaled[idx], ylim_scaled[idx]])
                ax[idx].set_yticks(
                    [-ylim[idx], -ylim[idx] / 2, 0.0, ylim[idx] / 2, ylim[idx]]
                )
                ax[idx].legend(loc='lower right', fontsize=20)
                ax[idx].grid(visible=True, which='both', color='k', alpha=1.0)
                for item in (
                    [ax[idx].title, ax[idx].xaxis.label, ax[idx].yaxis.label]
                    + ax[idx].get_xticklabels()
                    + ax[idx].get_yticklabels()
                ):
                    item.set_fontsize(30)
            fig.set_figheight(15)
            fig.set_figwidth(20)
            self.addVisualization(
                ReportFigure(fig, Caption=f'{title}: Phase Portrait')
            )

        def plot_angles_rates_latvel(
            collectionIndex=[], title="", xlim=[0, 1000], ylim=[0.5, 0.02, 3]
        ):
            fig, ax = plt.subplots(3, 1)
            ylim_scaled = np.array(ylim)
            ylim_scaled *= 1.1
            for cIdx in collectionIndex:
                time, data = self.comp_body_ang[cIdx]
                ax[0].plot(
                    time.squeeze(),
                    data[:, 0].convertUnits('degrees').squeeze(),
                    'r',
                    label='Roll',
                )
                ax[0].plot(
                    time.squeeze(),
                    data[:, 1].convertUnits('degrees').squeeze(),
                    'g',
                    label='Pitch',
                )
                ax[0].plot(
                    time.squeeze(),
                    data[:, 2].convertUnits('degrees').squeeze(),
                    'b',
                    label='Yaw',
                )
                time, data = self.TSData.body_ang_vel_this[cIdx]
                ax[1].plot(
                    time.squeeze(),
                    data[:, 0].convertUnits('degrees/s').squeeze(),
                    'r',
                    label='X',
                )
                ax[1].plot(
                    time.squeeze(),
                    data[:, 1].convertUnits('degrees/s').squeeze(),
                    'g',
                    label='Y',
                )
                ax[1].plot(
                    time.squeeze(),
                    data[:, 2].convertUnits('degrees/s').squeeze(),
                    'b',
                    label='Z',
                )
                time, data = self.TSData.deltaLatVelDP[cIdx]
                ax[2].plot(
                    time.squeeze(),
                    data[:, 0].convertUnits('mm/s').squeeze(),
                    'r',
                    label='X',
                )
                ax[2].plot(
                    time.squeeze(),
                    data[:, 1].convertUnits('mm/s').squeeze(),
                    'g',
                    label='Y',
                )
                ax[2].plot(
                    time.squeeze(),
                    data[:, 2].convertUnits('mm/s').squeeze(),
                    'b',
                    label='Z',
                )
            ax[0].set_ylabel("Body Attitude RPY (deg)")
            ax[1].set_ylabel("Body Rates (deg/s)")
            ax[2].set_ylabel("DP Delta-Lateral Vel (mm/s)")
            for idx in range(3):
                ax[idx].set_xlabel("Time (s)")
                ax[idx].set_xlim(xlim)
                ax[idx].set_xticks(range(xlim[0], xlim[1] + 1, 100))
                ax[idx].set_ylim([-ylim_scaled[idx], ylim_scaled[idx]])
                ax[idx].set_yticks(
                    [-ylim[idx], -ylim[idx] / 2, 0.0, ylim[idx] / 2, ylim[idx]]
                )
                ax[idx].legend(loc='lower right', fontsize=20)
                ax[idx].grid(visible=True, which='both', color='k', alpha=1.0)
                for item in (
                    [ax[idx].title, ax[idx].xaxis.label, ax[idx].yaxis.label]
                    + ax[idx].get_xticklabels()
                    + ax[idx].get_yticklabels()
                ):
                    item.set_fontsize(30)
            fig.set_figheight(25)
            fig.set_figwidth(20)
            self.addVisualization(
                ReportFigure(fig, Caption=f'{title}: Body Attitude and Rates')
            )

        # RUN_00_baseline
        self.addVisualization(ReportSection(self.name[0], Level=2, NewPage=True))
        plot_perturbation_angles_rates(collectionIndex=[0], title=self.name[0], xlim=[0, 1000])

        # RUN_01_subscribe_perturbation_only
        self.addVisualization(ReportSection(self.name[1], Level=2, NewPage=True))
        plot_perturbation_angles_rates(collectionIndex=[1], title=self.name[1], xlim=[0, 1000])
        plot_perturbation_angles_rates(collectionIndex=[1], title=self.name[1], xlim=[0, 200])
        plot_phase_portrait(
            collectionIndex=[1], title=self.name[1], xlim=[0.55, 0.55, 0.55], ylim=[0.025, 0.025, 0.025],
        )
        # RUN_02_subscribe_maneuver_only
        self.addVisualization(ReportSection(self.name[2], Level=2, NewPage=True))
        plot_mnvr_attitude(collectionIndex=[2], title=self.name[2], xlim=[0, 1000])

        # RUN_03_subscribe_both
        self.addVisualization(ReportSection(self.name[3], Level=2, NewPage=True))
        plot_perturbation_angles_rates(
            collectionIndex=[3], title=self.name[3], xlim=[0, 1000]
        )
        plot_mnvr_attitude(
            collectionIndex=[3], title=self.name[3], xlim=[0, 1000]
        )
        plot_body_attitude(
            collectionIndex=[3], title=self.name[3], xlim=[0, 1000]
        )
  

        # RUN_04_subscribe_perturb_mid_mnvr
        self.addVisualization(ReportSection(self.name[4], Level=2, NewPage=True))
        plot_perturbation_angles_rates(
            collectionIndex=[4], title=self.name[4], xlim=[0, 1000]
        )
        plot_mnvr_attitude(
            collectionIndex=[4], title=self.name[4], xlim=[0, 1000]
        )
        plot_body_attitude(
            collectionIndex=[4], title=self.name[4], xlim=[0, 1000]
        )

        # RUN_05_unsubscribe_perturbation
        self.addVisualization(ReportSection(self.name[5], Level=2, NewPage=True))
        plot_perturbation_angles_rates(
            collectionIndex=[5], title=self.name[5], xlim=[0, 1000]
        )

        # RUN_06_unsubscribe_maneuver
        self.addVisualization(ReportSection(self.name[6], Level=2, NewPage=True))
        plot_mnvr_attitude(
            collectionIndex=[6], title=self.name[6], xlim=[0, 1000]
        )
        # RUN_07_enforce_lat_vel
        self.addVisualization(ReportSection(self.name[7], Level=2, NewPage=True))
        plot_perturbation_angles_rates(collectionIndex=[7], title=self.name[7], xlim=[0, 1000])
        plot_angles_rates_latvel(
            collectionIndex=[7], title=self.name[7], xlim=[0, 1000]
        )

        # RUN_08_smoothing
        self.addVisualization(ReportSection(self.name[8], Level=2, NewPage=True))
        plot_perturbation_angles_rates(collectionIndex=[8], title=self.name[8], xlim=[0, 1000])
        plot_mnvr_attitude(collectionIndex=[8], title=self.name[8], xlim=[0, 1000])


        # RUN_09_random_rates
        self.addVisualization(ReportSection(self.name[9], Level=2, NewPage=True))
        plot_perturbation_angles_rates(
            collectionIndex=[9], title=self.name[9], xlim=[0, 1000]
        )
        plot_phase_portrait(
            collectionIndex=[9], title=self.name[9], xlim=[0.55, 5.6, 0.55], ylim=[0.02, 0.02, 0.02],
        )

        # RUN_10_random_rates_smoothing
        self.addVisualization(ReportSection(self.name[10], Level=2, NewPage=True))
        plot_perturbation_angles_rates(
            collectionIndex=[10], title=self.name[10], xlim=[0, 1000]
        )
        plot_phase_portrait(
            collectionIndex=[10], title=self.name[10], xlim=[0.55, 5.6, 0.55], ylim=[0.02, 0.02, 0.02],
        )

        # RUN_11_lvlh_fixed
        self.addVisualization(ReportSection(self.name[11], Level=2, NewPage=True))
        plot_perturbation_angles_rates(
            collectionIndex=[11], title=self.name[11], xlim=[0, 1000]
        )
        plot_body_attitude(
            collectionIndex=[11], title=self.name[11], xlim=[0, 1000]
        )
        plot_inertial_to_ref_attitude(
            collectionIndex=[11], title=self.name[11], xlim=[0, 1000]
        )

    #########################################
    # METHOD: _defineUnitTest
    #########################################
    def _defineUnitTest():
        """
        Unit tests for Analysis
        """
        pass
