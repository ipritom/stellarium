/*
 * Stellarium Telescope Control Plug-in
 * 
 * Copyright (C) 2006 Johannes Gajdosik
 * Copyright (C) 2009-2011 Bogdan Marinov
 * 
 * This module was originally written by Johannes Gajdosik in 2006
 * as a core module of Stellarium. In 2009 it was significantly extended with
 * GUI features and later split as an external plug-in module by Bogdan Marinov.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

#ifndef _TELESCOPE_CLIENT_HPP_
#define _TELESCOPE_CLIENT_HPP_

#include <QList>
#include <QString>
#include <QObject>

#include "StelObject.hpp"

//! returns the current system time in microseconds since the Epoch.
//! Prior to revision 6308, it was necessary to put put this method in an
//! #ifdef block, as duplicate function definition caused errors during static
//! linking.
qint64 getNow(void);

class StelCore;

enum Equinox {
	EquinoxJ2000,
	EquinoxJNow
};

//! \defgroup telescope-markers Telescope marker classes
//! \ingroup plugin-devicecontrol
//! StelObject classes that describe telescope reticles. Also act as
//! connection clients.

//! An abstract base class that should never be used directly, only inherited.
//! Describes a telescope object as a Stellarium object that can be displayed
//! as a reticle in the position where the connected telescope is currently
//! pointing.
//! This class used to be called Telescope, but it has been renamed
//! to TelescopeClient in order to resolve a compiler/linker conflict
//! with the identically named Telescope class in Stellarium's main code.
//! \ingroup telescope-markers
//! \todo Split off in a TelescopeControl namespace?
//! \todo Handle FOV circle change at any moment.
class TelescopeClient : public QObject, public StelObject
{
	Q_OBJECT
public:
	virtual ~TelescopeClient() {}
	
	// Method inherited from StelObject
	QString getEnglishName() const {return name;}
	QString getNameI18n() const {return nameI18n;}
	Vec3f getInfoColor() const;
	//! TelescopeClient supports the following InfoStringGroup flags:
	//! - Name
	//! - RaDecJ2000
	//! - RaDec
	//! - PlainText
	//! @param core the StelCore object
	//! @param flags a set of InfoStringGroup items to include in the return value.
	//! @return a QString containing an HMTL encoded description of the Telescope.
	QString getInfoString(const StelCore* core, const InfoStringGroup& flags) const;
	QString getType(void) const {return "Telescope";}
	virtual double getAngularSize(const StelCore*) const {Q_ASSERT(0); return 0;}	// TODO
		
	// Methods specific to telescope
	virtual void telescopeGoto(const Vec3d &j2000Pos) = 0;
	virtual bool isConnected(void) const = 0;
	virtual bool isInitialized(void) const {return true;}
	virtual bool hasKnownPosition(void) const = 0;
	//! Add a FOV indicator, a circle with the given angular size.
	//! There can be multiple indicators for a TelescopeClient.
	//! \param fov circle diameter in angular degrees.
	void addFovCircle(double fov) {if (fov>=0.0) fovCircles.push_back(fov);}
	//!  Get a list of FOV indicators.
	const QList<double>& getFovCircles() const {return fovCircles;}
	//! Clears the list of FOV indicators.
	void resetFovCircles() { fovCircles.clear(); }
	
	virtual bool prepareCommunication() {return false;}
	virtual void performCommunication() {}
	
signals:
	//! 
	void connectionEstablished();
	//! 
	void connectionLost();

protected:
	TelescopeClient(const QString &name);
	QString nameI18n;
	const QString name;
private:
	float getSelectPriority(const StelCore* core) const {Q_UNUSED(core); return -10.f;}
private:
	//! List of FOV values: circle diameter in angular degrees.
	QList<double> fovCircles;
};

typedef QSharedPointer<TelescopeClient> TelescopeClientP;

#endif // _TELESCOPE_CLIENT_HPP_
