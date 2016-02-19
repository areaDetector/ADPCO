/*
 * sisoboards.h
 *
 * Copyright (c) 2009,2010 Silicon Software GmbH, All Rights Reserved.
 */

/**
 * \file sisoboards.h
 *
 * \brief Board type constant definitions.
 *
 * \author Silicon Software GmbH
 */

#ifndef _SISOBOARDS_H
#define _SISOBOARDS_H

/**
 * Board type identifiers.
 *
 * Every device is identified by one of these identifiers. They
 * can be queried e.g. by Fg_getBoardType() or with
 * Fg_getParameterWithType() using the #FG_BOARD_INFORMATION argument.
 *
 * Please note that subvariants of the boards that are only different by the
 * physical layout of the connectors (e.g. CL connectors vs. PoCL connectors)
 * use the same type value.
 */
enum siso_board_type {
	PN_MICROENABLE = 0xa1,                         /**< microEnable I */
	PN_MICROENABLEII = 0xa2,                       /**< microEnable II */
	PN_MICROENABLE3I = 0xa3,                       /**< microEnable III */
	PN_MICROENABLE3IXXL = 0xa31,                   /**< microEnable III-XXL */
	PN_MICROENABLE4AD1CL = 0xa40,                  /**< microEnable IV AD1-CL */
	PN_MICROENABLE4BASE = PN_MICROENABLE4AD1CL,    /**< \deprecated old name for PN_MICROENABLE4AD1CL, maintained only for source compatibility */
	PN_MICROENABLE4BASEx4 = 0xa43,
	PN_MICROENABLE4AD4CL = 0xa42,                  /**< microEnable IV AD4-CL */
	PN_MICROENABLE4VD1CL = 0xa41,                  /**< microEnable IV VD1-CL */
	PN_MICROENABLE4FULLx1 = PN_MICROENABLE4VD1CL,  /**< \deprecated old name for PN_MICROENABLE4VD1CL, maintained only for source compatibility */
	PN_MICROENABLE4VD4CL = 0xa44,                  /**< microEnable IV VD4-CL */
	PN_MICROENABLE4FULLx4 = PN_MICROENABLE4VD4CL,  /**< \deprecated old name for PN_MICROENABLE4VD4CL, maintained only for source compatibility */
	PN_MICROENABLE4AS1CL = 0xa45,                  /**< microEnable IV AS1-CL */

	PN_MICROENABLE5CLHS1	= 0xa50,                  /**< microEnable V CL-HS1		*/
	PN_MICROENABLE5CXP4		= 0xa51,                  /**< microEnable V CXP4		*/
	PN_MICROENABLE5CXP4W5	= 0xa52,                  /**< microEnable V CXP4W5		*/

	PN_MICROENABLE4_H264CLx1  = 0xb41,               /**< kappa h264 Fujitsu MB86H51  */
	PN_MICROENABLE4_H264pCLx1 = 0xb42,		         /**< kappa h264 Fujitsu MB86H46A */

	PN_MICROENABLE4VQ4GE = 0xe44,                  /**< microEnable IV VQ4-GE */
	PN_MICROENABLE4GIGEx4 = PN_MICROENABLE4VQ4GE,  /**< \deprecated old name for PN_MICROENABLE4VQ4GE, maintained only for source compatibility */
	PN_MICROENABLE4AQ4GE = 0xe42,                  /**< microEnable IV AQ4-GE */

	PN_PX100 = 0xc41,                              /**< PixelPlant PX100 */
	PN_PX200 = 0xc42,                              /**< PixelPlant PX200 */
	PN_PX210 = 0xc43,                              /**< PixelPlant PX210-CL */
	PN_PX300 = 0xc44,                              /**< PixelPlant PX300-CxP */
	
#ifdef PLATFORM_PROTOTYPING
	PN_ML605 = 0xff46,                             /**< Xilinx ML605 Evaluation board */
#endif

	PN_UNKNOWN = 0xffff,
	PN_NONE    = 0
};

#endif
